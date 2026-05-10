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
#include "NetworkSocketMgr/concurrentqueue.h"
#include "Opcodes/Opcodes.h"
#include <openssl/rand.h>



class World {
public:
    static World& Instance();

    void Start();
    void Stop();
    void Update(); // main world tick


    void EnqueuePacket(std::shared_ptr<WorldSession> session, WorldPacket pkt);
    moodycamel::ConcurrentQueue<QueuedPacket> m_concurrentqueue;
    std::unordered_map<uint32_t, std::unique_ptr<Map>> m_maps;   // mapId -> Map

    Map* GetMap(uint32_t mapId);   // creates on demand

private:

    World() = default;
    void Run();
    void RegisterOpcodeHandlers();


    //Major O(n)2 jobs, these will be thread pools.
  
    //Calculate Visibility Sets. 
    void CalculateInVisibleRange_Players();
    void SendPlayerEntityUpdates();
    void SendMovementUpdates();
    //Calculate Visibilty of Objects in range of players.
    void CalculateInVisibleRange_Objects();
    //Calculate Visibility Sets for Creatures in range of Players.
    void CalculateInVisibleRange_Creatures();

    //Outgoing Packet Functions

    //Send out all of the packet updates

    void SendPrimaryTickUpdates();
    void SendPlayerUpdates();
    void SendCreatureUpdates();
    void SendObjectUpdates();
     

    void HandlePlayerVisibility(std::shared_ptr<Player> player);


    void HandleLocationRotation(std::shared_ptr<WorldSession> session, WorldPacket& pkt);

    void Handle_CMSG_PING(std::shared_ptr<WorldSession> session, WorldPacket& pkt);
    void Handle_CMSG_AUTH(std::shared_ptr<WorldSession> session, WorldPacket& pkt);
    void Handle_CMSG_AUTH_CHALLENGE(std::shared_ptr<WorldSession> session, WorldPacket& pkt);


    //Send SMSG Functions
    
    // Server Opcode to indicate when a client needs to spawn another player Entity. 
    void Send_SMSG_PLAYER_ENTITY_SPAWN(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer);
    void Send_SMSG_PLAYER_ENTITY_DESPAWN(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer);

    void Send_SMSG_UPDATE_PLAYER_CREATURE_LOCATION_ROTATION(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> TargetPlayer);

    void Send_SMSG_OBJECT_SPAWN(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer);
    void Send_SMSG_OBJECT_DESPAWN(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer);

    void Send_SMSG_OBJECT_UPDATE(std::shared_ptr<WorldSession> session, std::shared_ptr<Player> targetPlayer);

   


    float Distance(const Position& a, const Position& b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }

    uint64_t GenerateSecureSalt()
    {
        uint64_t Salt = 0;

        RAND_bytes(
            reinterpret_cast<unsigned char*>(&Salt),
            sizeof(Salt)
        );

        return Salt;
    }

    std::string GenerateAuthHash(const std::string& Password, uint64_t Salt)
    {
        std::string Combined =
            Password + std::to_string(Salt);

        return SHA256String(Combined);
    }

    std::string SHA256String(const std::string& Input);


    void InitDB();
    void StopDB();

    void processCallbacks();
    void UpdatePlayerVisibility(std::shared_ptr<WorldSession> session);


    std::thread m_thread;
    std::atomic<bool> m_running{false};

    std::deque<QueuedPacket> m_queue;
    std::mutex m_queueMutex;

    std::unordered_map<uint16_t, std::function<void(std::shared_ptr<WorldSession>, WorldPacket&)>> m_handlers;
};