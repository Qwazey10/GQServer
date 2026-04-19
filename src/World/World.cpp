//
// Created by michael on 4/6/26.
//

#include "World.h"
#include "WorldSessionMgr.h"
#include <iostream>
#include <chrono>

#include "TimeManager/TimeManager.h"

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

    TimeManager::Instance().ScheduleRepeating(15000, []() {
    std::cout << "Repeating every 15s - Print Ping for all Players: " << TimeManager::Instance().GetUptimeSeconds() << "s" << std::endl;});
    WorldSessionMgr::Instance().PingAllConnectedPlayers();

}

void World::Stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void World::EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket pkt) {
    m_concurrentqueue.enqueue(QueuedPacket{std::move(session), std::move(pkt)});
}


void World::Run() {
    while (m_running) {
        QueuedPacket qp;
        // Bulk dequeue is even faster
        while (m_concurrentqueue.try_dequeue(qp)) {
            auto it = m_handlers.find(qp.packet.GetOpcode());
            if (it != m_handlers.end()) {
                it->second(qp.session, qp.packet);
            } else {
                std::cout << "Unknown opcode: 0x" << std::hex << qp.packet.GetOpcode() << "\n";
            }
        }

        Update();
        TimeManager::Instance().Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void World::RegisterOpcodeHandlers() {


    m_handlers[CMSG_UPDATE_PLAYER_LOCATION_ROTATION] = [this](auto s, auto& p) { HandleLocationRotation(s, p); };
    m_handlers[CMSG_PING] = [this](auto s, auto& p) { HandlePing(s, p);};
}

void World::HandleLocationRotation(std::shared_ptr<WorldSession> session, WorldPacket& pkt) {
    float x = 0, y = 0, z = 0, yaw = 0, pitch = 0, roll = 0;
    pkt >> x >> y >> z >> yaw >> pitch >> roll;                     // easy to add more fields later

    session->GetPlayer()->SetPosition(x, y, z);
    session->GetPlayer()->SetRotation(pitch, yaw, roll);

    std::cout << "[World] Player " << session->GetPlayerId()
              << " moved to (" << x << ", " << y << ", " << z << ")"
                " Rot ("<< yaw <<","<< pitch << "," << roll <<")\n";

    // Broadcast to others
    WorldPacket broadcast(SMSG_LOCATION_UPDATE);
    broadcast << session->GetPlayerId() << x << y << z;

    WorldSessionMgr::Instance().BroadcastPacket(broadcast, session->GetPlayerId());
}
void World::HandlePing(std::shared_ptr<WorldSession> session, WorldPacket& pkt) {
    std::cout << "PING Message recieved\n";

    WorldPacket Newpkt(SMSG_PONG);
    WorldSessionMgr::Instance().SendPacketToSession(session,Newpkt);
}

void World::Update() {
    const float VISIBILITY_RANGE = 500.0f;

    //Get the array of connected sessions
    auto sessions = WorldSessionMgr::Instance().GetSessions();

    for (auto& sessionA : sessions)
    {
        auto playerA = sessionA->GetPlayer();
        if (!playerA)
            continue;

        std::unordered_set<int> newSet;

        for (auto& sessionB : sessions)
        {
            if (sessionA == sessionB)
                continue;

            auto playerB = sessionB->GetPlayer();
            if (!playerB)
                continue;

            if (playerA->zoneId_ != playerB->zoneId_)
                continue;

            float dist = Distance(playerA->GetPosition(), playerB->GetPosition());

            if (dist <= VISIBILITY_RANGE)
                newSet.insert(playerB->GetId());
        }

        // 🔒 Lock player's visibility set
        std::lock_guard<std::mutex> lock(playerA->m_inRangeMutex);

        // --- ENTERED RANGE ---
        for (int id : newSet)
        {
            if (!playerA->m_inRangePlayers.count(id))
            {
                auto targetSession = WorldSessionMgr::Instance().GetSessionByPlayerID(id);
                if (!targetSession) continue;

                auto targetPlayer = targetSession->GetPlayer();

                std::cout << "[World] Player " << playerA->GetId() << " entered range of player " << targetPlayer->GetId() << "\n";
                WorldPacket spawn(SMSG_PLAYER_SPAWN);
                spawn << targetPlayer->GetId()
                      << targetPlayer->GetPosition().x
                      << targetPlayer->GetPosition().y
                      << targetPlayer->GetPosition().z;

                sessionA->SendPacket(spawn);
            }
        }

        // --- LEFT RANGE ---
        for (int id : playerA->m_inRangePlayers)
        {
            if (!newSet.count(id))
            {
                WorldPacket despawn(SMSG_PLAYER_DESPAWN);
                despawn << id;
                sessionA->SendPacket(despawn);
            }
        }

        // Replace old set
        playerA->m_inRangePlayers = std::move(newSet);
    }
}