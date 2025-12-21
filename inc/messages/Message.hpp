#pragma once
#include <ctime>
#include <cstdint>
#include <cassert>
#include <vector>
#include <transport/TransportTypes.hpp>
#include <stdexcept>

struct Message
{
    uint8_t len;
    uint32_t idx;
    std::vector<char> data; 

    std::vector<transport::Byte> serialize() const {
        std::vector<transport::Byte> buffer;
        buffer.push_back(static_cast<transport::Byte>(data.size()) + sizeof(uint32_t));

        buffer.push_back((idx >> 24) & 0xFF);
        buffer.push_back((idx >> 16) & 0xFF);
        buffer.push_back((idx >> 8) & 0xFF);
        buffer.push_back((idx >> 0) & 0xFF);

        buffer.insert(buffer.end(), data.begin(), data.end());
        return buffer;
    }
  
    static Message deserialize(const std::vector<transport::Byte>& buffer) {
        if (buffer.size() < 5) {
            throw std::runtime_error("Buffer too small for deserialization");
        }

        Message msg;
        msg.len = buffer[0];
        
        msg.idx = (static_cast<uint32_t>(buffer[1]) << 24) |
                  (static_cast<uint32_t>(buffer[2]) << 16) |
                  (static_cast<uint32_t>(buffer[3]) << 8)  |
                  (static_cast<uint32_t>(buffer[4]) << 0);
        
        msg.data.assign(buffer.begin() + 5, buffer.end());
        
        if (msg.len != msg.data.size() + sizeof(uint32_t)) {
            throw std::runtime_error("Length field mismatch");
        }
        
        return msg;
    }

    static Message deserialize(const transport::Byte* rx_buff, size_t buff_len) {
        if (buff_len < 5) {
            throw std::runtime_error("Buffer too small:  need at least 5 bytes");
        }

        Message msg;
        
        msg.len = rx_buff[0];
        
        if (buff_len < (1 + msg.len)) {
            throw std::runtime_error("Incomplete message in buffer");
        }
        
        msg.idx = (static_cast<uint32_t>(rx_buff[1]) << 24) |
                  (static_cast<uint32_t>(rx_buff[2]) << 16) |
                  (static_cast<uint32_t>(rx_buff[3]) << 8)  |
                  (static_cast<uint32_t>(rx_buff[4]) << 0);
        
        size_t data_len = msg.len - sizeof(uint32_t);
        msg.data.assign(rx_buff + 5, rx_buff + 5 + data_len);
        
        return msg;
    }

    void print() const {
        printf("Message:\n");
        printf("  len: %u\n", len);
        printf("  idx:  0x%08X (%u)\n", idx, idx);
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
