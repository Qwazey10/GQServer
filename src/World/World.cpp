#include "World.h"
#include "WorldSessionMgr.h"
#include <iostream>
#include <chrono>

#include "TimeManager/TimeManager.h"
#include "SQL/DatabaseManager.h"
#include <random>
#include <cfloat>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

#include "CombatManager/CombatManager.h"
#include "WorldGridManager/WorldGridManager.h"

World& World::Instance() {
    static World instance;
    return instance;
}

void World::Start() {
    if (m_running) return;
    RegisterOpcodeHandlers();
    m_running = true;
    m_thread = std::thread(&World::Run, this);
    std::cout << "World thread started (60 TPS)\n";

    /*//Start Timer Manager to ping connected players.
    TimeManager::Instance().ScheduleRepeating(10000, []() {
    std::cout << "Repeating every 10s - Ping Clients: " << TimeManager::Instance().GetUptimeSeconds() << "s" << std::endl;});
    WorldSessionMgr::Instance().PingAllConnectedPlayers();*/
}

void World::Stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void World::EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket pkt) {
    m_concurrentqueue.enqueue(QueuedPacket{std::move(session), std::move(pkt)});
}


void World::Run()
{
    constexpr int TICK_RATE_MS = 100;

    while (m_running)
    {
        auto TickStart = std::chrono::steady_clock::now();

        // Process incoming packets
        QueuedPacket qp;

        while (m_concurrentqueue.try_dequeue(qp))
        {
            auto it = m_handlers.find(qp.packet.GetOpcode());

            if (it != m_handlers.end())
            {
                try
                {
                    it->second(qp.session, qp.packet);
                }
                catch (const std::exception& e)
                {
                    std::cout << "Packet handler exception: "
                        << e.what() << "\n";

                    qp.session->Disconnect();
                }
                catch (...)
                {
                    std::cout << "Unknown packet handler exception\n";
                    qp.session->Disconnect();
                }
            }
            else
            {
                std::cout << "Unknown opcode: 0x"
                    << std::hex
                    << qp.packet.GetOpcode()
                    << std::dec
                    << "\n";
            }
        }
        //Process Database Callbacks
        DatabaseManager::Instance().ProcessCallbacks();
        // World Systems
        Update();

        TimeManager::Instance().Update();

        // Measure elapsed
        auto TickEnd = std::chrono::steady_clock::now();

        auto Elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                TickEnd - TickStart
            );

        auto SleepTime =
            std::chrono::milliseconds(TICK_RATE_MS) - Elapsed;

        if (SleepTime.count() > 0)
        {
            std::this_thread::sleep_for(SleepTime);
        }
        else
        {
            std::cout << "[World] Tick overloaded by "
                << -SleepTime.count()
                << " ms\n";
        }
    }
}

void World::RegisterOpcodeHandlers() {
    m_handlers[CMSG_UPDATE_PLAYER_LOCATION_ROTATION] = [this](auto s, auto& p) { HandleLocationRotation(s, p); };
    m_handlers[CMSG_PING] = [this](auto s, auto& p) { Handle_CMSG_PING(s, p);};

    m_handlers[CMSG_AUTH] = [this](auto s, auto& p) { Handle_CMSG_AUTH(s, p);};
    m_handlers[CMSG_AUTH_CHALLENGE] = [this](auto s, auto& p) {Handle_CMSG_AUTH_CHALLENGE(s, p);};

    m_handlers[CMSG_PLAYER_REQUEST_AUTOATTACK_TOGGLE] = [this](auto s, auto& p) {Handle_CMSG_PLAYER_REQUEST_AUTOATTACK_TOGGLE(s, p);};

    m_handlers[CMSG_PLAYER_TARGET_REQUEST] = [this](auto s, auto& p) {Handle_CMSG_PLAYER_TARGET_REQUEST(s, p);};

    m_handlers[CMSG_CHAT_REQUEST] = [this](auto s, auto& p) {Handle_CMSG_CHAT_REQUEST(s, p);};
}

void World::Update()
{


    //
    // PROFILER STATE
    //
    static int TickCounter = 0;

    static double MinMs = DBL_MAX;
    static double MaxMs = 0.0;
    static double TotalMs = 0.0;

    //
    // START TIMER
    //
    auto StartTime = std::chrono::high_resolution_clock::now();

    //
    // WORLD SYSTEMS
    //

    WorldGridManager::Instance().Update(100.0f);
    //Calculate O(n2) Player Visibility Range

    //CalculateInVisibleRange_Players();

    //Combat Manager
    CombatManager::Instance().Update((100.0f));
    // Spawn / Despawn visibility updates
    SendPlayerUpdates();

    // Movement replication
    SendMovementUpdates();

    //
    // END TIMER
    //
    auto EndTime = std::chrono::high_resolution_clock::now();

    double ElapsedMs =
        std::chrono::duration<double, std::milli>(
            EndTime - StartTime
        ).count();

    //
    // TRACK STATS
    //
    TickCounter++;

    TotalMs += ElapsedMs;

    if (ElapsedMs < MinMs)
    {
        MinMs = ElapsedMs;
    }

    if (ElapsedMs > MaxMs)
    {
        MaxMs = ElapsedMs;
    }

    //
    // PRINT EVERY 50 TICKS
    //
    constexpr int PrintInterval = 50;

    if (TickCounter >= PrintInterval)
    {
        double AverageMs = TotalMs / TickCounter;

        std::cout
            << "\n========== WORLD UPDATE PROFILER ==========\n"
            << "Ticks Sampled : " << TickCounter << "\n"
            << "Min Tick (ms) : " << MinMs << "\n"
            << "Max Tick (ms) : " << MaxMs << "\n"
            << "Avg Tick (ms) : " << AverageMs << "\n"
            << "===========================================\n"
            << std::endl;

        //
        // RESET SAMPLE WINDOW
        //
        TickCounter = 0;

        MinMs = DBL_MAX;
        MaxMs = 0.0;
        TotalMs = 0.0;
    }
}


void World::Handle_CMSG_AUTH(std::shared_ptr<WorldSession> session, WorldPacket& pkt) {
    std::string username;

    //pull username out of packet.
    pkt >> username;

    //Expand this to get the auth_login database later on, right now its just "testplayer" for what we expect.
    std::string ExpectedUsername = "testplayer";

    std::cout << "[Auth] Auth request from username: " << username << "\n";

    if (username == ExpectedUsername)
    {
        std::cout << "[CMSG] - Auth -- Username matches what we expect, respond with SMSG_AUTH" << username << "==" << ExpectedUsername << "\n";

        
        uint64_t salt = GenerateSecureSalt();
        //cache the salt in the session for later use
        session->SetAuthSalt(salt);

        WorldPacket response(SMSG_AUTH);
        response << username << salt;   // echo username + send salt

        session->SendPacket(response);
        std::cout << "[Auth] Sent SMSG_AUTH with salt: " << salt << "\n";
        return;
    }
    else
    {
        std::cout << "{CMSG) - Auth -- Username DOES NOT match , respond with SMSG_AUTH_FAIL" << username << "!=" << ExpectedUsername << "\n";
        WorldPacket authfail(SMSG_AUTH_FAIL);
        session->SendPacket(authfail);
        return;
    }
}

void World::Handle_CMSG_AUTH_CHALLENGE(
    std::shared_ptr<WorldSession> session,
    WorldPacket& pkt)
{
    std::string username;
    std::string clientHash;

    pkt >> username >> clientHash;

    uint64_t serverSalt = session->GetAuthSalt();

    std::cout << "[Auth] Challenge received from "
        << username << "\n";

    if (username != "testplayer")
    {
        WorldPacket fail(SMSG_AUTH_CHALLENGE_FAIL);

        std::string reason = "Invalid Username";

        fail << reason;

        session->SendPacket(fail);

        return;
    }

    std::string expectedHash =
        GenerateAuthHash(
            "password123",
            serverSalt
        );

    std::cout << "[Auth] ClientHash: "
        << clientHash << "\n";

    std::cout << "[Auth] ExpectedHash: "
        << expectedHash << "\n";

    if (clientHash != expectedHash)
    {
        WorldPacket fail(SMSG_AUTH_CHALLENGE_FAIL);

        std::string reason = "Invalid Password";

        fail << reason;

        session->SendPacket(fail);

        return;
    }

    std::cout << "[Auth] SUCCESS\n";

    session->SetAuthenticated(true);

    session->GetPlayer()->characterName_ = username;

    WorldPacket success(SMSG_AUTH_CHALLENGE);

    success << username;

    session->SendPacket(success);
}



std::string World::SHA256String(const std::string& Input)
{
    {
        unsigned char Hash[SHA256_DIGEST_LENGTH];

        SHA256(
            reinterpret_cast<const unsigned char*>(Input.c_str()),
            Input.size(),
            Hash
        );

        std::stringstream ss;

        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        {
            ss << std::hex
                << std::setw(2)
                << std::setfill('0')
                << (int)Hash[i];
        }

        return ss.str();
    }
}


void World::HandleLocationRotation(std::shared_ptr<WorldSession> session, WorldPacket& pkt) {
    float x = 0, y = 0, z = 0, yaw = 0;
    pkt >> x >> y >> z >> yaw;                    
    
    session->GetPlayer()->SetPosition(x, y, z);
    session->GetPlayer()->SetRotation(yaw);

    /*std::cout << "[World] Player " << session->GetPlayerId()
        << " moved to (" << x << ", " << y << ", " << z << ")"
        " Rotation (" << yaw  << ")\n";*/
}

void World::Handle_CMSG_PING(std::shared_ptr<WorldSession> session, WorldPacket &pkt) {
    std::cout << "PING Message recieved\n";

    WorldPacket Newpkt(SMSG_PONG);
    WorldSessionMgr::Instance().SendPacketToSession(session, Newpkt);
}


void World::Handle_CMSG_PLAYER_REQUEST_AUTOATTACK_TOGGLE(std::shared_ptr<WorldSession> session, WorldPacket &pkt) {

    std::cout <<"AUTO ATTACK OFF MESSAGE RECIEVED\n";

    WorldPacket AtkOn(SMSG_PLAYER_AUTOATTACK_ON);
    WorldPacket AtkOff(SMSG_PLAYER_AUTOATTACK_OFF);


    /*spawn << targetPlayer->GetGUID()
        << targetPlayer->GetName()
        << targetPlayer->GetPosition().x
        << targetPlayer->GetPosition().y
        << targetPlayer->GetPosition().z
        << targetPlayer->GetRotation(); //Character Yaw*/
    if (session) {
        auto PlayerRef = session->GetPlayer();

        if (PlayerRef) {

            //Player is Not attacking, send Atk On PAcket and set attack to 1
            if (PlayerRef->AttackState == 0) {

                PlayerRef->AttackState = 1;
                session->SendPacket(AtkOn);
                return;
            }

            //Player is Attacking, send AtkOff Packet and set attack to 0
            if (PlayerRef->AttackState == 1) {

                PlayerRef->AttackState = 0;
                session->SendPacket(AtkOff);
                return;
            }
        }

    }

}

void World::Handle_CMSG_PLAYER_TARGET_REQUEST(std::shared_ptr<WorldSession> session, WorldPacket &pkt) {
    //check if target is within existing grids

    int64_t GUID = 0;

    pkt >> GUID;

    std::cout << "[Player] Target Request received GUID " << GUID << "\n";

    auto Player = session->GetPlayer();

    if (Player) {
        Player->TargetGUID = GUID;
        //TODO - Check if the player ID is inside the visible grid.
    }
    WorldPacket sndpkt(SMSG_PLAYER_TARGET_REQUEST);

    // if we approve the packet, send the requested target back, right now we just auto approve.

    sndpkt << GUID;

    std::cout << "[Player] Target Request Sent " << GUID << "\n";

    session->SendPacket(sndpkt);
}

/*
 *CHAT HANLDERS
 */
void World::Handle_CMSG_CHAT_REQUEST(std::shared_ptr<WorldSession> session, WorldPacket& pkt)
{
    std::string raw;
    pkt >> raw;

    // Basic safety
    if (raw.empty() || raw.length() > 255)
        return;

    auto sender = session->GetPlayer();
    if (!sender)
        return;

    // Optional: rate-limit / spam protection here

    // Trim (in case client didn't)
    // (simple version – replace with a proper trim if you have one)
    size_t start = raw.find_first_not_of(" \t\r\n");
    size_t end   = raw.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
        return;
    raw = raw.substr(start, end - start + 1);

    // -------------------------------------------------
    // Parse command
    // -------------------------------------------------
    std::string lower = raw;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

    // No slash → normal say
    if (raw[0] != '/')
    {
        Send_SMSG_CHAT_SAY(session, raw);
        return;
    }

    // Split into command + rest
    std::string command;
    std::string message;

    size_t space = raw.find(' ');
    if (space != std::string::npos)
    {
        command = lower.substr(0, space);               // already lower-cased
        message = raw.substr(space + 1);                // keep original case

        // trim leading spaces of message
        size_t msgStart = message.find_first_not_of(" \t");
        if (msgStart != std::string::npos)
            message = message.substr(msgStart);
        else
            message.clear();
    }
    else
    {
        command = lower;                                // just "/say"
        message.clear();
    }

    if (message.empty() && command != "/tell" /* etc. if you want pure commands */)
        return; // nothing to say

    // -------------------------------------------------
    // Dispatch
    // -------------------------------------------------
    if (command == "/say" || command == "/s")
    {
        Send_SMSG_CHAT_SAY(session, message);

    }
    else if (command == "/shout" || command == "/y")
    {
        Send_SMSG_CHAT_SHOUT(session, message);
       // ***** BroadcastShout(sender, message);
    }
    else if (command == "/guild" || command == "/g")
    {
        // TODO: once you have guilds
        //  *****BroadcastGuild(sender, message);
        Send_SMSG_CHAT_GUILD(session, message);
    }
    else if (command == "/tell" || command == "/t" ||
             command == "/w"    || command == "/whisper")
    {
        // Expect: /tell PlayerName message...
        size_t nameSpace = message.find(' ');
        if (nameSpace == std::string::npos)
            return; // no target name

        std::string targetName = message.substr(0, nameSpace);
        std::string tellMsg    = message.substr(nameSpace + 1);

        // trim tellMsg
        size_t tStart = tellMsg.find_first_not_of(" \t");
        if (tStart != std::string::npos)
            tellMsg = tellMsg.substr(tStart);
        else
            return;

        //****SendTell(sender, targetName, tellMsg);
    }
    else
    {
        // Unknown command – you can ignore or treat as say
        // BroadcastSay(sender, raw);
    }
}

void World::Send_SMSG_CHAT_SAY(std::shared_ptr<WorldSession> session,
                               const std::string& message)
{
    auto Sender = session->GetPlayer();
    if (!Sender)
        return;

    const Vector3& senderPos = Sender->GetPosition();

    // Build the packet once
    WorldPacket saypkt(SMSG_CHAT_SAY);
    saypkt << Sender->characterName_ << message;

    // Echo back to the sender
    session->SendPacket(saypkt);
    std::cout << "Say Message (self): " << Sender->characterName_
              << " " << message << "\n";

    // Send to nearby visible players
    for (int32_t targetGUID : Sender->currentVisiblePlayers_)
    {
        auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(targetGUID);
        if (!targetSession)
            continue;

        auto targetPlayer = targetSession->GetPlayer();
        if (!targetPlayer)
            continue;

        float dist = Vector3::Distance(senderPos, targetPlayer->GetPosition());
        if (dist < SAY_DISTANCE)
        {
            targetSession->SendPacket(saypkt);
            std::cout << "Say Message Received: " << targetPlayer->characterName_
                      << " " << message << "\n";
        }
    }
}

void World::Send_SMSG_CHAT_SHOUT(std::shared_ptr<WorldSession> session, std::string message) {

    auto Sender = session->GetPlayer();
    if (!Sender)
        return;

    const Vector3& senderPos = Sender->GetPosition();

    // Build the packet once
    WorldPacket saypkt(SMSG_CHAT_SHOUT);
    saypkt << Sender->characterName_ << message;

    // Echo back to the sender
    session->SendPacket(saypkt);
    std::cout << "Say Message (self): " << Sender->characterName_
              << " " << message << "\n";

    // Send to nearby visible players
    for (int32_t targetGUID : Sender->currentVisiblePlayers_)
    {
        auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(targetGUID);
        if (!targetSession)
            continue;

        auto targetPlayer = targetSession->GetPlayer();
        if (!targetPlayer)
            continue;

        float dist = Vector3::Distance(senderPos, targetPlayer->GetPosition());
        if (dist < SAY_DISTANCE)
        {
            targetSession->SendPacket(saypkt);
            std::cout << "Shout Message Received: " << targetPlayer->characterName_
                      << " " << message << "\n";
        }
    }

}

void World::Send_SMSG_CHAT_GUILD(std::shared_ptr<WorldSession> session, std::string message) {

    std::cout << "GUILD Message Received: " << " " << message << "\n";
}

void World::Send_SMSG_CHAT_TELL(std::shared_ptr<WorldSession> session, std::string targetName,
    std::string message) {

    std::cout << "TELL Message Received: " << "Tell Target:" << " message "<< " <<\n";

    WorldPacket tellpkt(SMSG_CHAT_TELL);
    tellpkt << targetName << message;

    if (session) {
        session->SendPacket(tellpkt);
    }

}




void World::SendPlayerUpdates()
{
    auto sessions = WorldSessionMgr::Instance().GetSessions();

    for (auto& session : sessions)
    {
        auto player = session->GetPlayer();
        if (!player) continue;

        // DESPAWNS: Players who were in previousVisible, but are NOT in currentVisible
        for (int32_t oldGUID : player->previousVisiblePlayers_)
        {
            if (player->currentVisiblePlayers_.find(oldGUID) == player->currentVisiblePlayers_.end())
            {
                auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(oldGUID);
                if (targetSession && targetSession->GetPlayer())
                {
                    Send_SMSG_PLAYER_ENTITY_DESPAWN(session, targetSession->GetPlayer());
                }
            }
        }

        // SPAWNS: Players who are in currentVisible, but were NOT in previousVisible
        for (int32_t newGUID : player->currentVisiblePlayers_)
        {
            if (player->previousVisiblePlayers_.find(newGUID) == player->previousVisiblePlayers_.end())
            {
                auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(newGUID);
                if (targetSession && targetSession->GetPlayer())
                {
                    Send_SMSG_PLAYER_ENTITY_SPAWN(session, targetSession->GetPlayer());
                }
            }
        }
    }
}

void World::SendMovementUpdates()
{
    auto sessions = WorldSessionMgr::Instance().GetSessions();

    for (auto& session : sessions)
    {
        auto player = session->GetPlayer();
        if (!player) continue;

        for (int32_t visibleGUID : player->currentVisiblePlayers_)
        {
            auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerGUID(visibleGUID);
            if (!targetSession || !targetSession->GetPlayer()) continue;

            // Optional: You could add a check here to ensure the target actually moved
            // before sending a packet to save bandwidth.

            Send_SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION(
                session,
                targetSession->GetPlayer()
            );
        }
    }
}


void World::CalculateInVisibleRange_Objects() {
    //Do the same for this loop. Should i create a separate creature manager class
    //How does azerothcore handle creature and AI structure wise.

}
void World::CalculateInVisibleRange_Creatures() {
    //Same for players but do a loop over possible creatures
}
void World::Send_SMSG_PLAYER_ENTITY_SPAWN(
    std::shared_ptr<WorldSession> session,
    std::shared_ptr<Player> targetPlayer)
{
    WorldPacket spawn(SMSG_PLAYER_ENTITY_SPAWN);

    spawn << targetPlayer->GetGUID()
        << targetPlayer->GetName()
        << targetPlayer->GetPosition().x
        << targetPlayer->GetPosition().y
        << targetPlayer->GetPosition().z
        << targetPlayer->GetRotation(); //Character Yaw

    session->SendPacket(spawn);
}

void World::Send_SMSG_PLAYER_ENTITY_DESPAWN(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer)
{
    int TargetPlayerID = targetPlayer->GetGUID();
    WorldPacket despawn(SMSG_PLAYER_ENTITY_DESPAWN);
    despawn << TargetPlayerID;
    session->SendPacket(despawn);
}

void World::Send_SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION(
    std::shared_ptr<WorldSession> session,
    std::shared_ptr<Player> TargetPlayer)
{
    uint64_t timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

    WorldPacket pkt(SMSG_UPDATE_PLAYER_ENTITY_LOCATION_ROTATION);

    pkt << TargetPlayer->GetGUID()
        << TargetPlayer->GetName()
        << TargetPlayer->GetPosition().x
        << TargetPlayer->GetPosition().y
        << TargetPlayer->GetPosition().z
        << TargetPlayer->GetRotation()
        << timestamp;

        //Debug Print, don't delete! -M
/*    std::cout << "[SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION] "
        << "PlayerID: " << TargetPlayer->GetId()
        << " | Name: " << TargetPlayer->GetName()
        << " | Pos: ("
        << TargetPlayer->GetPosition().x << ", "
        << TargetPlayer->GetPosition().y << ", "
        << TargetPlayer->GetPosition().z << ")"
        << " | Rot: " << TargetPlayer->GetRotation()
        << " | Timestamp: " << timestamp
        << std::endl;*/

    session->SendPacket(pkt);
}
