#include "WorldSession.h"
#include "World.h"
#include <iostream>

#include "WorldSessionMgr.h"

WorldSession::WorldSession(asio::ip::tcp::socket socket, int playerId)
    : m_socket(std::move(socket)), m_playerId(playerId), m_player(std::make_shared<Player>(playerId)) {}

WorldSession::~WorldSession() { Disconnect(); }

void WorldSession::Start() {
    ReadHeader();
}

void WorldSession::ReadHeader() {
    asio::async_read(m_socket, asio::buffer(m_headerBuf),
        [self = shared_from_this()](asio::error_code ec, std::size_t) {
            if (ec) { self->Disconnect(); return; }

            uint16_t payloadSize = *reinterpret_cast<uint16_t*>(self->m_headerBuf.data());
            uint16_t opcode = *reinterpret_cast<uint16_t*>(self->m_headerBuf.data() + 2);

            self->m_payloadBuf.resize(payloadSize);
            self->ReadPayload(opcode);
        });
}

void WorldSession::ReadPayload(uint16_t opcode) {
    asio::async_read(m_socket, asio::buffer(m_payloadBuf),
        [self = shared_from_this(), opcode](asio::error_code ec, std::size_t) {
            if (ec) { self->Disconnect(); return; }

            WorldPacket pkt(opcode, std::move(self->m_payloadBuf));
            World::Instance().EnqueuePacket(self, std::move(pkt));

            self->ReadHeader(); // continue reading next packet
        });
}

void WorldSession::SendPacket(WorldPacket pkt) {
    uint16_t payloadSize = static_cast<uint16_t>(pkt.GetData().size());
    std::vector<uint8_t> buffer(4 + payloadSize);

    *reinterpret_cast<uint16_t*>(buffer.data()) = payloadSize;
    *reinterpret_cast<uint16_t*>(buffer.data() + 2) = pkt.GetOpcode();
    std::copy(pkt.GetData().begin(), pkt.GetData().end(), buffer.begin() + 4);

    asio::async_write(m_socket, asio::buffer(buffer),
        [self = shared_from_this()](asio::error_code ec, std::size_t) {
            if (ec) self->Disconnect();
        });
}

void WorldSession::Disconnect() {
    if (m_socket.is_open()) {
        asio::error_code ec;
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        m_socket.close(ec);
        std::cout << "[Session " << m_playerId << "] Disconnected\n";
        WorldSessionMgr::Instance().RemoveSessionByPlayerID(m_playerId);
    }
}