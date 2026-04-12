#include "Map.h"
#include "WorldSession.h"   // for SendPacket
#include "Opcodes.h"
#include "World.h"

Map::Map(uint32_t mapId) : m_mapId(mapId) {}

Map::~Map() { Stop(); }

void Map::Start() {
    m_running = true;
    m_thread = std::thread(&Map::Run, this);
}

void Map::Stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void Map::EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket&& pkt) {
    std::lock_guard<std::mutex> lock(m_packetMutex);
    m_packetQueue.push_back({std::move(session), std::move(pkt)});
}

void Map::Run() {
    while (m_running) {
        auto now = GetCurrentMs();   // your existing time helper
        uint32_t diff = now - m_lastUpdateTime;
        if (diff >= 50) {            // ~20 TPS minimum, adjust as needed
            Update(diff);
            m_lastUpdateTime = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Map::Update(uint32_t diff) {
    // 1. Process incoming packets
    {
        std::lock_guard<std::mutex> lock(m_packetMutex);
        while (!m_packetQueue.empty()) {
            auto& qp = m_packetQueue.front();
            // Call opcode handler (you'll register handlers per Map or globally)
            // For simplicity, you can keep a global handler map for now
            World::Instance().HandlePacket(qp.session, std::move(qp.packet));  // or local handler
            m_packetQueue.pop_front();
        }
    }

    // 2. Update monsters AI
    for (auto& monster : m_monsters) {
        monster->UpdateAI(static_cast<float>(diff) / 1000.0f);
    }

    // 3. Visibility + updates (player <-> player and player <-> monster)
    const float VIS_RANGE = 60.0f;

    for (const auto& [pid, player] : m_players) {
        // Player-to-player visibility (keep your existing logic, just scoped to this map)
        for (const auto& [otherId, other] : m_players) {
            if (pid == otherId) continue;
            // calculate distance, send spawn/despawn if needed...
        }

        // Player-to-monster visibility
        for (const auto& monster : m_monsters) {
            float dist = Distance(player->GetPosition(), monster->GetPosition());
            bool inRange = dist <= VIS_RANGE;

            // You need a per-player "known monsters" set to avoid spamming
            // For sketch: assume simple check or add std::unordered_set<uint64_t> knownMonsters per player

            if (inRange) {
                WorldPacket spawn(SMSG_MONSTER_SPAWN);
                spawn << monster->GetId() << monster->GetEntry()
                      << monster->GetPosition().x << monster->GetPosition().y << monster->GetPosition().z;
                player->GetSession()->SendPacket(spawn);   // or queue it
            }
        }
    }
}