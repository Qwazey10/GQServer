#ifndef GQUESTSERVER_WORLDSESSION_H
#define GQUESTSERVER_WORLDSESSION_H

#pragma once
#include <asio.hpp>
#include <memory>
#include "Player.h"
#include "Packets/WorldPacket.h"

class WorldSession : public std::enable_shared_from_this<WorldSession> {
public:
    explicit WorldSession(asio::ip::tcp::socket socket, int playerId);
    ~WorldSession();

    void Start();
    void SendPacket(WorldPacket pkt);
    void Disconnect();

    int GetPlayerId() const { return m_playerId; }
    std::shared_ptr<Player> GetPlayer() { return m_player; }

    Get

private:
    void ReadHeader();
    void ReadPayload(uint16_t payloadSize);

    asio::ip::tcp::socket m_socket;
    int m_playerId = -1;
    std::shared_ptr<Player> m_player;

    std::array<uint8_t, 4> m_headerBuf{}; // payloadSize(2) + opcode(2)
    std::vector<uint8_t> m_payloadBuf;
};


#endif //GQUESTSERVER_WORLDSESSION_H