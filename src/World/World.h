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

    void HandleLocationRotation(std::shared_ptr<WorldSession> session, WorldPacket& pkt);

    static void Handle_CMSG_PING(std::shared_ptr<WorldSession> session, WorldPacket& pkt);
    void Handle_CMSG_AUTH(std::shared_ptr<WorldSession> session, WorldPacket& pkt);
    void Handle_CMSG_AUTH_CHALLENGE(std::shared_ptr<WorldSession> session, WorldPacket& pkt);
    

/*    CMSG_AUTH = 0x0001, // Authcode the client sends to request the password Hash
        SMSG_AUTH = 0x0002, // AuthCode the server sends which will contain the Hash the client will salt. 

        CMSG_AUTH_CHALLENGE = 0x0003, // Auth code the client will send containing password + hash
        SMSG_AUTH_CHALLENGE = 0x0004, // Authcode the server will send telling the client authentication was successful. 
        SMSG_AUTH_CHALLENGE_FAIL = 0x0005, // Authcode the server will send if the client authentication FAILED.*/

    float Distance(const Position& a, const Position& b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }


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