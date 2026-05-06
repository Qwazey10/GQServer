#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

// For std::bit_cast (C++20)
#include <bit>

class WorldPacket
{
public:
    WorldPacket() = default;
    explicit WorldPacket(uint16_t opcode) : m_opcode(opcode) {}
    WorldPacket(uint16_t opcode, std::vector<uint8_t> data)
        : m_opcode(opcode), m_data(std::move(data)) {
    }

    uint16_t GetOpcode() const { return m_opcode; }
    const std::vector<uint8_t>& GetData() const { return m_data; }

    // ========================
    // WRITE
    // ========================
    template<typename T>
    WorldPacket& operator<<(T value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "POD only");

        if constexpr (std::is_same_v<T, bool>)
        {
            uint8_t b = value ? 1 : 0;
            m_data.insert(m_data.end(), &b, &b + 1);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            uint32_t bits = std::bit_cast<uint32_t>(value);
            bits = htonl(bits);
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&bits);
            m_data.insert(m_data.end(), ptr, ptr + sizeof(uint32_t));
        }
        else if constexpr (sizeof(T) == 2)
        {
            value = htons(static_cast<uint16_t>(value));  // safe cast for signed
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
            m_data.insert(m_data.end(), ptr, ptr + sizeof(T));
        }
        else if constexpr (sizeof(T) == 4)
        {
            value = htonl(static_cast<uint32_t>(value));
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
            m_data.insert(m_data.end(), ptr, ptr + sizeof(T));
        }
        else
        {
            // 64-bit and others - no endian swap for now
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
            m_data.insert(m_data.end(), ptr, ptr + sizeof(T));
        }
        return *this;
    }

    WorldPacket& operator<<(const std::string& str)
    {
        uint16_t len = static_cast<uint16_t>(str.size());
        *this << len;

        if (len > 0)
            m_data.insert(m_data.end(), str.begin(), str.end());

        return *this;
    }

    // ========================
    // READ
    // ========================
    template<typename T>
    WorldPacket& operator>>(T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>, "POD only");

        if (m_readPos + sizeof(T) > m_data.size())
            throw std::runtime_error("Read past end");

        if constexpr (std::is_same_v<T, bool>)
        {
            value = (m_data[m_readPos] != 0);
            m_readPos += 1;
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            uint32_t bits = 0;
            std::memcpy(&bits, m_data.data() + m_readPos, sizeof(uint32_t));
            m_readPos += sizeof(uint32_t);

            bits = ntohl(bits);
            value = std::bit_cast<T>(bits);
        }
        else
        {
            std::memcpy(&value, m_data.data() + m_readPos, sizeof(T));
            m_readPos += sizeof(T);

            if constexpr (sizeof(T) == 2)
                value = ntohs(static_cast<uint16_t>(value));
            else if constexpr (sizeof(T) == 4)
                value = ntohl(static_cast<uint32_t>(value));
        }
        return *this;
    }

    WorldPacket& operator>>(std::string& str)
    {
        uint16_t len = 0;
        *this >> len;

        if (m_readPos + len > m_data.size())
            throw std::runtime_error("Read past end");

        str.assign(reinterpret_cast<const char*>(m_data.data() + m_readPos), len);
        m_readPos += len;

        return *this;
    }

private:
    uint16_t m_opcode = 0;
    std::vector<uint8_t> m_data;
    size_t m_readPos = 0;
};