//
// Created by michael on 4/6/26.
//

#include "World.h"
#include "WorldSessionMgr.h"
#include <iostream>
#include <chrono>

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
}

void World::Stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void World::EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket pkt) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_queue.emplace_back(QueuedPacket{std::move(session), std::move(pkt)});
}

void World::Run() {
    while (m_running) {
        std::deque<QueuedPacket> local;
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            local.swap(m_queue);
        }

        for (auto& qp : local) {
            auto it = m_handlers.find(qp.packet.GetOpcode());
            if (it != m_handlers.end()) {
                it->second(qp.session, qp.packet);
            } else {
                std::cout << "Unknown opcode: 0x" << std::hex << qp.packet.GetOpcode() << "\n";
            }
        }

        Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 TPS
    }
}

void World::RegisterOpcodeHandlers() {
    m_handlers[CMSG_LOCATION_ROTATION] = [this](auto s, auto& p) { HandleLocationRotation(s, p); };
}

void World::HandleLocationRotation(std::shared_ptr<WorldSession> session, WorldPacket& pkt) {
    float x = 0, y = 0, z = 0;
    pkt >> x >> y >> z;                     // easy to add more fields later

    session->GetPlayer()->SetPosition(x, y, z);

    std::cout << "[World] Player " << session->GetPlayerId()
              << " moved to (" << x << ", " << y << ", " << z << ")\n";

    // Broadcast to others
    WorldPacket broadcast(SMSG_LOCATION_UPDATE);
    broadcast << session->GetPlayerId() << x << y << z;

    WorldSessionMgr::Instance().BroadcastPacket(broadcast, session->GetPlayerId());
}

void World::Update() {
    // Put your main game logic here (physics, AI, timers, etc.)
}