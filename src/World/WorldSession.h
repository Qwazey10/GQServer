#ifndef GQUESTSERVER_WORLDSESSION_H
#define GQUESTSERVER_WORLDSESSION_H

#pragma once
#include <asio.hpp>
#include <memory>
#include "Player.h"
#include "Recast.h"
#include "Packets/WorldPacket.h"
constexpr uint16_t MAX_PACKET_SIZE = 8192; // 8KB sanity limit

class WorldSession : public std::enable_shared_from_this<WorldSession> {
public:
    explicit WorldSession(asio::ip::tcp::socket socket, int playerId);
    ~WorldSession();

    void Start();
    void SendPacket(const WorldPacket& pkt);
    void Disconnect();

    int GetPlayerId() const { return m_playerId; }
    std::shared_ptr<Player> GetPlayer() { return m_player; }

    bool IsAuthenticated() const { return bAuthenticated; }
    void SetAuthenticated(bool val) { bAuthenticated = val; }

    uint64_t GetAuthSalt() const { return m_authSalt; }
    void SetAuthSalt(uint64_t salt) { m_authSalt = salt; }

    uint32_t GetAccountID() const {return AccountID;}

private:
    void ReadHeader();
    void ReadPayload(uint16_t payloadSize);

    bool bAuthenticated = false;
    uint64_t m_authSalt = 0;

    uint32_t AccountID = 0;

    asio::ip::tcp::socket m_socket;
    int m_playerId = -1;
    std::shared_ptr<Player> m_player;

    std::array<uint8_t, 4> m_headerBuf{}; // payloadSize(2) + opcode(2)
    std::vector<uint8_t> m_payloadBuf;
};


#endif //GQUESTSERVER_WORLDSESSION_H