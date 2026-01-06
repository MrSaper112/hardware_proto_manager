#pragma once
#include <ctime>
#include <cstdint>
#include <cassert>
#include <vector>
#include <transport/TransportTypes.hpp>
#include <stdexcept>
#include "MessageTypes.hpp"

namespace wm::messages
{
    class Message
    {
    public:
        uint8_t len;
        uint32_t idx;
        MessageType mesType;
        VectorChar data;

        Message() : len(0), idx(0), mesType(MessageType::Undefined) {};

        Message(uint32_t index, MessageType type) : idx(index), mesType(type)
        {
            this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType));
        }

        template <typename T>
        Message(uint32_t index, MessageType type, const T &payload) : idx(index), mesType(type), data(payload)
        {
            this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType) + data.get().size());
        }

        std::vector<char> serialize() const
        {
            std::vector<char> buffer;
            buffer.push_back(static_cast<char>(data.get().size()) + sizeof(idx) + sizeof(mesType));
            buffer.push_back(static_cast<uint8_t>(mesType));

            buffer.push_back((idx >> 24) & 0xFF);
            buffer.push_back((idx >> 16) & 0xFF);
            buffer.push_back((idx >> 8) & 0xFF);
            buffer.push_back((idx >> 0) & 0xFF);

            buffer.insert(buffer.end(), data.get().begin(), data.get().end());
            return buffer;
        }

        static Message deserialize(const std::vector<char> &buffer)
        {
            auto msg = deserialize(buffer.data(), buffer.size());
            return msg;
        }

        static Message deserialize(const char *rxBuff, size_t buffLen)
        {
            if (buffLen < m_preambleSize)
                throw std::runtime_error("Buffer too small:  need at least 5 bytes");
            if (buffLen > 255)
                throw std::runtime_error("Buffer too large: maximum size is 255 bytes");

            Message msg;

            msg.len = rxBuff[0];

            msg.mesType = intToMessageType(rxBuff[1]);

            msg.idx = (static_cast<uint32_t>(rxBuff[2]) << 24) |
                      (static_cast<uint32_t>(rxBuff[3]) << 16) |
                      (static_cast<uint32_t>(rxBuff[4]) << 8) |
                      (static_cast<uint32_t>(rxBuff[5]) << 0);

            std::vector<char> vec;
            vec.assign(rxBuff + m_preambleSize, rxBuff + msg.len + 1);
            msg.data = VectorChar(vec);

            return msg;
        };

        void print() const
        {
            printf("Message:\n");
            printf("  len: %u\n", len);
            printf("  idx:  0x%08X (%u)\n", idx, idx);
            printf("  message_type:  0x%08X (%u-%s)\n", static_cast<uint8_t>(mesType), static_cast<uint8_t>(mesType), messageTypeToString(mesType));
            printf("  data (%zu bytes): ", data.get().size());
            for (auto c : data.get())
            {
                printf("%02X ", static_cast<uint8_t>(c));
            }
            printf("\n  data (ASCII): ");
            for (auto c : data.get())
            {
                if (c >= 32 && c <= 126)
                {
                    printf("%c", c);
                }
                else
                {
                    printf(".");
                }
            }
            printf("\n");
        }

        bool operator==(const Message &mes)
        {
            return this->idx == mes.idx && this->len == mes.len;
        }

    private:
        static constexpr uint8_t m_preambleSize = sizeof(len) + sizeof(mesType) + sizeof(idx);
    };
}