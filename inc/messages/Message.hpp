#pragma once
#include <ctime>
#include <cstdint>
#include <cassert>
#include <vector>
#include <transport/TransportTypes.hpp>
#include <stdexcept>
#include "MessageTypes.hpp"

struct Message
{
    uint8_t len;
    uint32_t idx;
    MessageType mesType;
    VectorChar data;

    Message() : len(0), idx(0), mesType(MessageType::Undefined) {};

    Message(uint32_t index, MessageType type): idx(index), mesType(type)
    {
        this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType));
    }

    Message(uint32_t index, MessageType type, const VectorChar &payload) : idx(index), mesType(type), data(payload)
    {
        this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType) + data.size());
    }

    Message(uint32_t index, MessageType type, const std::string &payload) : idx(index), mesType(type)
    {
        data.assign(payload.begin(), payload.end());
        this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType) + data.size());
    }

    std::vector<transport::Byte> serialize() const
    {
        std::vector<transport::Byte> buffer;
        buffer.push_back(static_cast<transport::Byte>(data.size()) + sizeof(idx) + sizeof(mesType));
        buffer.push_back(static_cast<uint8_t>(mesType));

        buffer.push_back((idx >> 24) & 0xFF);
        buffer.push_back((idx >> 16) & 0xFF);
        buffer.push_back((idx >> 8) & 0xFF);
        buffer.push_back((idx >> 0) & 0xFF);

        buffer.insert(buffer.end(), data.begin(), data.end());
        return buffer;
    }

    static Message deserialize(const std::vector<transport::Byte> &buffer)
    {
        auto msg = deserialize(buffer.data(), buffer.size());
        return msg;
    }

    static Message deserialize(const transport::Byte *rx_buff, size_t buff_len)
    {
        if (buff_len < 5)
        {
            throw std::runtime_error("Buffer too small:  need at least 5 bytes");
        }
        uint8_t offset = 0;

        Message msg;

        msg.len = rx_buff[offset];

        if (buff_len < (1 + msg.len))
        {
            throw std::runtime_error("Incomplete message in buffer");
        }

        msg.mesType = intToMessageType(rx_buff[++offset]);

        msg.idx = (static_cast<uint32_t>(rx_buff[++offset]) << 24) |
                  (static_cast<uint32_t>(rx_buff[++offset]) << 16) |
                  (static_cast<uint32_t>(rx_buff[++offset]) << 8) |
                  (static_cast<uint32_t>(rx_buff[++offset]) << 0);

        ++offset;

        size_t data_len = msg.len - sizeof(uint32_t);
        msg.data.assign(rx_buff + offset, rx_buff + offset + data_len);

        return msg;
    };

    void print() const
    {
        printf("Message:\n");
        printf("  len: %u\n", len);
        printf("  idx:  0x%08X (%u)\n", idx, idx);
        printf("  message_type:  0x%08X (%u-%s)\n", mesType, mesType, messageTypeToString(mesType));
        printf("  data (%zu bytes): ", data.size());
        for (auto c : data)
        {
            printf("%02X ", static_cast<uint8_t>(c));
        }
        printf("\n  data (ASCII): ");
        for (auto c : data)
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

    std::string getDataAsString() const
    {
        return std::string(data.begin(), data.end());
    }

    void setData(const std::string &str)
    {
        data.assign(str.begin(), str.end());
        len = static_cast<uint8_t>(sizeof(mesType) + sizeof(idx) + data.size());
    }

    bool operator==(const Message &mes)
    {
        return this->idx == mes.idx && this->len == mes.len;
    }
};
