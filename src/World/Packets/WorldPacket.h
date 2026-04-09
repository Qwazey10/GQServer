#ifndef GQUESTSERVER_PACKETS_H
#define GQUESTSERVER_PACKETS_H

#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>
#include <stdexcept>

class WorldPacket {
public:
    WorldPacket() = default;
    explicit WorldPacket(uint16_t opcode) : m_opcode(opcode) {}

    WorldPacket(uint16_t opcode, std::vector<uint8_t> data)
        : m_opcode(opcode), m_data(std::move(data)) {}

    uint16_t GetOpcode() const { return m_opcode; }
    size_t Size() const { return m_data.size(); }

    // Write operators
    template<typename T>
    WorldPacket& operator<<(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        m_data.insert(m_data.end(), ptr, ptr + sizeof(T));
        return *this;
    }

    WorldPacket& operator<<(const std::string& str) {
        uint16_t len = static_cast<uint16_t>(str.length());
        *this << len;
        if (len > 0) m_data.insert(m_data.end(), str.begin(), str.end());
        return *this;
    }

    // Read operators
    template<typename T>
    WorldPacket& operator>>(T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        if (m_readPos + sizeof(T) > m_data.size())
            throw std::runtime_error("WorldPacket: read past end");
        std::memcpy(&value, m_data.data() + m_readPos, sizeof(T));
        m_readPos += sizeof(T);
        return *this;
    }

    WorldPacket& operator>>(std::string& str) {
        uint16_t len = 0;
        *this >> len;
        if (len > 0) {
            if (m_readPos + len > m_data.size())
                throw std::runtime_error("WorldPacket: read past end");
            str.assign(reinterpret_cast<const char*>(m_data.data() + m_readPos), len);
            m_readPos += len;
        } else {
            str.clear();
        }
        return *this;
    }

    const std::vector<uint8_t>& GetData() const { return m_data; }

private:
    uint16_t m_opcode = 0;
    std::vector<uint8_t> m_data;
    size_t m_readPos = 0;
};
#endif //GQUESTSERVER_PACKETS_H