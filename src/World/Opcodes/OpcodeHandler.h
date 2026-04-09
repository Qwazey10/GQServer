/*#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include "World/Packets/WorldPacket.h"

class WorldSession;

using PacketHandler = std::function<void(std::shared_ptr, WorldPacket&)>;

class OpcodeRegistry {
public:
    static OpcodeRegistry& Instance() {
        static OpcodeRegistry instance;
        return instance;
    }

    void Register(uint16_t opcode, PacketHandler handler) {
        m_handlers[opcode] = handler;
    }

    void Handle(std::shared_ptr<WorldSession> session, WorldPacket& pkt) {
        auto it = m_handlers.find(pkt.GetOpcode());
        if (it != m_handlers.end()) {
            it->second(session, pkt);
        }
    }


private:
    std::unordered_map<uint16_t, PacketHandler> m_handlers;
};*/