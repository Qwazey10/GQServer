//
// Created by michael on 4/6/26.
//

#include "World.h"
#include "WorldSessionMgr.h"
#include <iostream>
#include <chrono>

#include "TimeManager/TimeManager.h"
#include <random>

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

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

    //Start Timer Manager to ping connected players.
    TimeManager::Instance().ScheduleRepeating(10000, []() {
    std::cout << "Repeating every 10s - Ping Clients: " << TimeManager::Instance().GetUptimeSeconds() << "s" << std::endl;});
    WorldSessionMgr::Instance().PingAllConnectedPlayers();

    TimeManager::Instance().ScheduleRepeating(10000, []() {
    std::cout << "World Timer -- Repeating every 15s - Print Ping for all Connected Player " << TimeManager::Instance().GetUptimeSeconds() << "s" << std::endl;
    WorldSessionMgr::Instance().PingAllConnectedPlayers();
        });
   

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
}



void World::Update() {

    //Thread Pool this later
    CalculateInVisibleRange_Players();
    //Send PlayerUpdates
    SendPlayerEntityUpdates();
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
    float x = 0, y = 0, z = 0, yaw = 0, pitch = 0, roll = 0;
    pkt >> x >> y >> z >> yaw >> pitch >> roll;                    
    
    session->GetPlayer()->SetPosition(x, y, z);
    session->GetPlayer()->SetRotation(pitch, yaw, roll);


    std::cout << "[World] Player " << session->GetPlayerId()
        << " moved to (" << x << ", " << y << ", " << z << ")"
        " Rot (" << yaw << "," << pitch << "," << roll << ")\n";

    /*// Broadcast to others
    WorldPacket broadcast(SMSG_LOCATION_UPDATE);
    broadcast << session->GetPlayerId() << x << y << z;*/

   // WorldSessionMgr::Instance().BroadcastPacket(broadcast, session->GetPlayerId());
}

void World::Handle_CMSG_PING(std::shared_ptr<WorldSession> session, WorldPacket &pkt) {
    std::cout << "PING Message recieved\n";

    WorldPacket Newpkt(SMSG_PONG);
    WorldSessionMgr::Instance().SendPacketToSession(session, Newpkt);
}


void World::CalculateInVisibleRange_Players() {

    constexpr float VISIBILITY_RANGE = 100000.0f;

    auto sessionsCopy = WorldSessionMgr::Instance().CopySessions();

    std::vector<std::shared_ptr<Player>> players;

    players.reserve(sessionsCopy.size());

    for (auto& session : sessionsCopy)
    {
        auto player = session->GetPlayer();

        if (player)
        {
            players.push_back(player);
        }
    }

    for (auto& playerA : players)
    {
        std::unordered_set<int> NewVisibleSet;

        for (auto& playerB : players)
        {
            if (playerA == playerB)
                continue;

            if (playerA->zoneId_ != playerB->zoneId_)
                continue;

            float dist =
                Distance(
                    playerA->GetPosition(),
                    playerB->GetPosition()
                );

            if (dist <= VISIBILITY_RANGE)
            {
                NewVisibleSet.insert(playerB->GetId());
            }
        }

        playerA->SetCacheInVisibilityRange(NewVisibleSet);
    }
}

void World::SendPlayerEntityUpdates()
{
    auto sessions = WorldSessionMgr::Instance().CopySessions();

    for (auto& session : sessions)
    {
        if (!session)
            continue;

        auto player = session->GetPlayer();

        if (!player)
            continue;

        // Copy sets safely
        auto CurrentSet = player->GetInVisibilityRange();
        auto CacheSet = player->GetCacheInVisibilityRange();

        //
        // ENTERED RANGE
        //
        for (int id : CacheSet)
        {
            if (!CurrentSet.count(id))
            {
                auto targetSession =
                    WorldSessionMgr::Instance().GetSessionByPlayerID(id);

                if (!targetSession)
                    continue;

                auto targetPlayer = targetSession->GetPlayer();

                if (!targetPlayer)
                    continue;

                Send_SMSG_PLAYER_ENTITY_SPAWN(
                    session,
                    targetPlayer
                );
            }
        }

        //
        // LEFT RANGE
        //
        for (int id : CurrentSet)
        {
            if (!CacheSet.count(id))
            {
                auto targetSession =
                    WorldSessionMgr::Instance().GetSessionByPlayerID(id);

                if (!targetSession)
                    continue;

                auto targetPlayer = targetSession->GetPlayer();

                if (!targetPlayer)
                    continue;

                Send_SMSG_PLAYER_ENTITY_DESPAWN(
                    session,
                    targetPlayer
                );
            }
        }

        //
        // MOVEMENT UPDATES
        //
        for (int id : CacheSet)
        {
            auto targetSession =
                WorldSessionMgr::Instance().GetSessionByPlayerID(id);

            if (!targetSession)
                continue;

            auto targetPlayer = targetSession->GetPlayer();

            if (!targetPlayer)
                continue;

            Send_SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION(session, targetPlayer);

    
        }

        //
        // COMMIT CACHE -> ACTIVE
        //
        player->SetInVisibilityRange(CacheSet);
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

    spawn << targetPlayer->GetId()
        << targetPlayer->GetName()
        << targetPlayer->GetPosition().x
        << targetPlayer->GetPosition().y
        << targetPlayer->GetPosition().z
        << targetPlayer->GetRotation().Pitch
        << targetPlayer->GetRotation().Yaw
        << targetPlayer->GetRotation().Roll;

    session->SendPacket(spawn);
}

void World::Send_SMSG_PLAYER_ENTITY_DESPAWN(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer)
{

    int TargetPlayerID = targetPlayer->GetId();
    WorldPacket despawn(SMSG_PLAYER_ENTITY_DESPAWN);
    despawn << TargetPlayerID;
    session->SendPacket(despawn);

}

void World::Send_SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> TargetPlayer)
{

    uint64_t timestamp; // add a timestamp here i am not sure which int or std:chrono to use
    WorldPacket LocationUpdatePacket(SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION);
    LocationUpdatePacket << TargetPlayer->GetId()
        << TargetPlayer->GetName()
        << TargetPlayer->GetPosition().x
        << TargetPlayer->GetPosition().y
        << TargetPlayer->GetPosition().z
        << TargetPlayer->GetRotation().Pitch
        << TargetPlayer->GetRotation().Yaw
        << TargetPlayer->GetRotation().Roll
        << timestamp;

        session->SendPacket(LocationUpdatePacket);
}
