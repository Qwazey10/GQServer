#pragma once
#include "Player.h"
#include "Creature.h"
#include <thread>
#include <mutex>
#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

#include "WorldSession.h"

struct QueuedPacket {
    std::shared_ptr<WorldSession> session;
    WorldPacket packet;
};

class Map {
public:
    explicit Map(uint32_t mapId);
    ~Map();

    void Start();                    // launches the thread
    void Stop();

    void EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket&& pkt);
    void AddPlayer(std::shared_ptr<Player> player);
    void RemovePlayer(uint64_t playerId);

    void SpawnMonster(uint32_t entry, float x, float y, float z);

private:
    void Run();                      // thread main loop
    void Update(uint32_t diff);      // main game tick

    uint32_t m_mapId;
    std::thread m_thread;
    std::atomic<bool> m_running{false};

    std::mutex m_packetMutex;
    std::deque<QueuedPacket> m_packetQueue;

    std::unordered_map<uint64_t, std::shared_ptr<Player>> m_players;   // playerId -> Player
    std::vector<std::unique_ptr<Creature>> m_monsters;

    uint32_t m_lastUpdateTime = 0;
};