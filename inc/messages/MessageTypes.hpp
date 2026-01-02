#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

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

class VectorChar
{
public:
    VectorChar() = default;

    VectorChar(const char *str)
        : m_data(str, str + std::strlen(str))
    {
    }

    VectorChar(const std::string &str)
        : m_data(str.begin(), str.end())
    {
    }

    VectorChar(uint8_t value)
        : m_data{static_cast<char>(value)}
    {
    }

    VectorChar(char value)
        : m_data{value}
    {
    }

    VectorChar(std::vector<char> &data)
        : m_data(data)
    {
    }

    VectorChar(const std::vector<uint8_t> &data)
    {
        m_data.reserve(data.size());
        for (uint8_t byte : data)
        {
            m_data.push_back(static_cast<char>(byte));
        }
    }

    template <typename T>
    VectorChar(const T &) = delete;

    std::vector<char> &&move() { return std::move(m_data); }
    std::vector<char>& get() { return m_data; }
    const std::vector<char>& get() const { return m_data; }
protected:
    std::vector<char> m_data;
};
