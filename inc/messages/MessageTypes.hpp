#pragma once

#include <cstdint>
#include <vector>

enum class MessageType : uint8_t
{
    None = 0x00,
    Command = 0x01,
    Response = 0x02,
    Data = 0x03,
    HeartBeat = 0x04,
    Error = 0x05,

    Undefined = 0xFF
};

constexpr MessageType intToMessageType(uint8_t v) noexcept
{
    if (v <= static_cast<uint8_t>(MessageType::Error))
    {
        return static_cast<MessageType>(v);
    }

    return MessageType::Undefined;
}

constexpr const char *messageTypeToString(MessageType type) noexcept
{
    switch (type)
    {
    case MessageType::None:
        return "NONE";
    case MessageType::Command:
        return "COMMAND";
    case MessageType::Response:
        return "RESPONSE";
    case MessageType::Data:
        return "DATA";
    case MessageType::Error:
        return "ERROR";
    case MessageType::Undefined:
        return "UNDEFINED";
    case MessageType::HeartBeat:
        return "HEARTBEAT";
    default:
        return "UNKNOWN";
    }
}

using VectorChar = std::vector<char>;
