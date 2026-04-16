#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include "WorldSession.h"

class WorldSessionMgr {
public:
    static WorldSessionMgr& Instance();

    void AddSession(std::shared_ptr<WorldSession> session);
    void RemoveSession(int playerId);
    void BroadcastPacket(const WorldPacket& pkt, int excludePlayerId = -1);

    std::shared_ptr<WorldSession> GetSessionByPlayerID(int playerId);

    std::vector<std::shared_ptr<WorldSession>> GetSessions()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_sessions; // copy (safe)
    }



private:
    WorldSessionMgr() = default;
    std::vector<std::shared_ptr<WorldSession>> m_sessions;
    std::mutex m_mutex;
};