#pragma once
#include <thread>
#include <atomic>
#include <cmath>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <functional>

#include "Map.h"
#include "Packets/WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes/Opcodes.h"



class World {
public:
    static World& Instance();

    void Start();
    void Stop();
    void Update(); // main world tick

    void EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket pkt);

    std::unordered_map<uint32_t, std::unique_ptr<Map>> m_maps;   // mapId -> Map

    Map* GetMap(uint32_t mapId);   // creates on demand

private:

    World() = default;
    void Run();
    void RegisterOpcodeHandlers();



    void HandleLocationRotation(std::shared_ptr<WorldSession> session, WorldPacket& pkt);

    float Distance(const Position& a, const Position& b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }



    std::thread m_thread;
    std::atomic<bool> m_running{false};

    std::deque<QueuedPacket> m_queue;
    std::mutex m_queueMutex;

    std::unordered_map<uint16_t, std::function<void(std::shared_ptr<WorldSession>, WorldPacket&)>> m_handlers;
};