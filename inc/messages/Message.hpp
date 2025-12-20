#pragma once
#include <ctime>
#include <cstdint>
#include <cassert>
#include <vector>
#include <transport/TransportTypes.hpp>

struct Message
{
    uint8_t idx;
    std::vector<char> data; 

    std::vector<transport::Byte> serialize() const {
        std::vector<transport::Byte> buffer;
        buffer.push_back(idx);
        buffer.push_back(static_cast<transport::Byte>(data.size()));
        buffer.insert(buffer.end(), data.begin(), data.end());
        return buffer;
    }
    
    static Message deserialize(const uint8_t* bytes, size_t len) {
        Message msg;
        msg.idx = bytes[0];
        uint8_t data_len = bytes[1];
        msg.data.assign(bytes + 2, bytes + 2 + data_len);
        return msg;
    }
};
