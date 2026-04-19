#include "WorldSessionMgr.h"

#include "Opcodes/Opcodes.h"

WorldSessionMgr& WorldSessionMgr::Instance() {
    static WorldSessionMgr instance;
    return instance;
}

void WorldSessionMgr::AddSession(std::shared_ptr<WorldSession> session) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.push_back(std::move(session));
}

void WorldSessionMgr::RemoveSessionByPlayerID(int playerId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.erase(std::remove_if(m_sessions.begin(), m_sessions.end(),
        [playerId](const auto& s) { return s->GetPlayerId() == playerId; }),
        m_sessions.end());
}

void WorldSessionMgr::RemoveSessionBySessionPtr(std::shared_ptr<WorldSession> session) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.erase(std::remove(m_sessions.begin(), m_sessions.end(), session), m_sessions.end());
}

void WorldSessionMgr::BroadcastPacket(const WorldPacket& pkt, int excludePlayerId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& session : m_sessions) {
        if (session->GetPlayerId() != excludePlayerId) {
            session->SendPacket(pkt);  // note: this makes a copy
        }
    }
}


void WorldSessionMgr::PingAllConnectedPlayers() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& session : m_sessions) {
        session->SendPacket(WorldPacket(SMSG_PONG));
    }
}

std::shared_ptr<WorldSession> WorldSessionMgr::GetSessionByPlayerID(int playerId) {

    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& session : m_sessions) {
        if (session->GetPlayerId() == playerId) {
            return session;
        }
    }
    return nullptr;
}

void WorldSessionMgr::SendPacketToSession(std::shared_ptr<WorldSession> session, const WorldPacket& pkt) {
    session->SendPacket(pkt);

}
