#pragma once
#include <ctime>
#include <cstdint>
#include <cassert>
#include <vector>
#include <transport/TransportTypes.hpp>
#include <stdexcept>

enum class MessageType : uint8_t
{
    NONE = 0,
    TEXT = 1, 
    CONFIG = 2,
    SENSOR_DATA = 3,
    NOTIFICATION = 4,
    ERROR = 5,

    UNDEFINED = 255
};

constexpr MessageType intToMessageType(uint8_t v) noexcept {
    if (v <= static_cast<uint8_t>(MessageType::ERROR)) {
        return static_cast<MessageType>(v);
    }

    return MessageType::UNDEFINED;
}

constexpr const char* messageTypeToString(MessageType type) noexcept {
    switch (type) {
        case MessageType::NONE:  return "NONE";
        case MessageType::TEXT: return "TEXT";
        case MessageType::CONFIG: return "CONFIG";
        case MessageType::SENSOR_DATA:  return "SENSOR_DATA";
        case MessageType::NOTIFICATION:  return "NOTIFICATION";
        case MessageType::ERROR: return "ERROR";
        case MessageType::UNDEFINED: return "UNDEFINED";
        default: return "UNKNOWN";
    }
}

using VectorChar = std::vector<char>;

struct Message
{
    uint8_t len;
    uint32_t idx;
    MessageType mesType;
    VectorChar data; 

    std::vector<transport::Byte> serialize() const {
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
  
    static Message deserialize(const std::vector<transport::Byte>& buffer) {
        auto msg = deserialize(buffer.data(), buffer.size());
        return msg;
    }

    static Message deserialize(const transport::Byte* rx_buff, size_t buff_len) {
        if (buff_len < 5) {
            throw std::runtime_error("Buffer too small:  need at least 5 bytes");
        }
        uint8_t offset = 0;

        Message msg;
        
        msg.len = rx_buff[offset];
        
        if (buff_len < (1 + msg.len)) {
            throw std::runtime_error("Incomplete message in buffer");
        }

        msg.mesType = intToMessageType(rx_buff[++offset]);

        msg.idx = (static_cast<uint32_t>(rx_buff[++offset]) << 24) |
                  (static_cast<uint32_t>(rx_buff[++offset]) << 16) |
                  (static_cast<uint32_t>(rx_buff[++offset]) << 8)  |
                  (static_cast<uint32_t>(rx_buff[++offset]) << 0);
        
        ++offset;

        size_t data_len = msg.len - sizeof(uint32_t);
        msg.data.assign(rx_buff + offset, rx_buff + offset + data_len);
        
        return msg;
    };

    void print() const {
        printf("Message:\n");
        printf("  len: %u\n", len);
        printf("  idx:  0x%08X (%u)\n", idx, idx);
        printf("  message_type:  0x%08X (%u-%s)\n", mesType, mesType, messageTypeToString(mesType));
        printf("  data (%zu bytes): ", data.size());
        for (auto c : data) {
            printf("%02X ", static_cast<uint8_t>(c));
        }
        printf("\n  data (ASCII): ");
        for (auto c : data) {
            if (c >= 32 && c <= 126) {
                printf("%c", c);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
};
