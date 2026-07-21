#include "WorldSession.h"
#include "World.h"
#include <iostream>
#include "WorldSessionMgr.h"

WorldSession::WorldSession(asio::ip::tcp::socket socket, int playerId)
    : m_socket(std::move(socket)), m_playerId(playerId), m_player(std::make_shared<Player>()) {}

WorldSession::~WorldSession() { Disconnect(); }

void WorldSession::Start() {
    ReadHeader();
}

void WorldSession::ReadHeader()
{
    asio::async_read(m_socket, asio::buffer(m_headerBuf),
        [self = shared_from_this()](asio::error_code ec, std::size_t)
        {
            if (ec)
            {
                self->Disconnect();
                return;
            }

            uint16_t payloadSize = 0;
            uint16_t opcode = 0;

            std::memcpy(&payloadSize, self->m_headerBuf.data(), sizeof(uint16_t));
            std::memcpy(&opcode, self->m_headerBuf.data() + 2, sizeof(uint16_t));

            payloadSize = ntohs(payloadSize);
            opcode = ntohs(opcode);

            // 🚨 sanity check (VERY IMPORTANT)
            if (payloadSize > MAX_PACKET_SIZE)
            {
                std::cout << "[Session " << self->m_playerId << "] Invalid packet size: " << payloadSize << "\n";
                self->Disconnect();
                return;
            }

            self->m_payloadBuf.resize(payloadSize);
            self->ReadPayload(opcode);
        });
}

void WorldSession::ReadPayload(uint16_t opcode)
{
    asio::async_read(m_socket, asio::buffer(m_payloadBuf),
        [self = shared_from_this(), opcode](asio::error_code ec, std::size_t)
        {
            if (ec)
            {
                self->Disconnect();
                return;
            }

            WorldPacket pkt(opcode, std::move(self->m_payloadBuf));
            World::Instance().EnqueuePacket(self, std::move(pkt));

            self->ReadHeader();
        });
}

void WorldSession::SendPacket(const WorldPacket& pkt)
{
    uint16_t payloadSize = static_cast<uint16_t>(pkt.GetData().size());

    auto buffer = std::make_shared<std::vector<uint8_t>>();
    buffer->resize(4 + payloadSize);

    uint16_t sizeNet = htons(payloadSize);
    uint16_t opcodeNet = htons(pkt.GetOpcode());

    std::memcpy(buffer->data(), &sizeNet, 2);
    std::memcpy(buffer->data() + 2, &opcodeNet, 2);
    std::memcpy(buffer->data() + 4, pkt.GetData().data(), payloadSize);

    asio::async_write(m_socket, asio::buffer(*buffer),
        [self = shared_from_this(), buffer](std::error_code ec, std::size_t)
        {
            if (ec)
                self->Disconnect();
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