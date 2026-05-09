#include "WorldSessionMgr.h"
#include "Opcodes/Opcodes.h"
#include <iostream>

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

std::vector<std::shared_ptr<WorldSession>> WorldSessionMgr::CopySessions()
{
    //Create Copy Vector
    std::vector<std::shared_ptr<WorldSession>> sessionsCopy;

    {
        //Lock Sessions Mutex
        std::lock_guard<std::mutex> lock(m_mutex);
        sessionsCopy = m_sessions; // copy
    }
    return sessionsCopy;
}


void WorldSessionMgr::PingAllConnectedPlayers()
{
    //Copy Sessions 
    std::vector<std::shared_ptr<WorldSession>> sessionsCopy = CopySessions();

    uint32_t timestamp = static_cast<uint32_t>(std::time(nullptr));

    for (const auto& session : sessionsCopy)
    {
        if (!session) continue;

        WorldPacket pkt(SMSG_PONG);
        pkt << timestamp;

        session->SendPacket(pkt);
    }
}



std::shared_ptr<WorldSession> WorldSessionMgr::GetSessionByPlayerID(int playerId) {

    std::vector<std::shared_ptr<WorldSession>> sessionsCopy = CopySessions();

   
    for (const auto& session : sessionsCopy) {
        if (session->GetPlayerId() == playerId) {
            return session;
        }
    }
    return nullptr;
}

void WorldSessionMgr::SendPacketToSession(std::shared_ptr<WorldSession> session, const WorldPacket& pkt) {
    session->SendPacket(pkt);
}
