#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

namespace wm::messages
{
    /**
     * @enum MessageType
     * @brief Enumeration of available message types in the protocol.
     * 
     * Defines the different categories of messages that can be transmitted
     * in the communication protocol.
     */
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

    /**
     * @brief Converts an unsigned integer to a MessageType enum.
     * 
     * Safely converts a numeric value to the corresponding MessageType enum value.
     * If the value is not a valid message type, returns MessageType::Undefined.
     * 
     * @param v The integer value to convert.
     * 
     * @return The corresponding MessageType or MessageType::Undefined if invalid.
     */
    constexpr MessageType intToMessageType(uint8_t v) noexcept
    {
        if (v <= static_cast<uint8_t>(MessageType::Error))
        {
            return static_cast<MessageType>(v);
        }

        return MessageType::Undefined;
    }

    /**
     * @brief Converts a MessageType enum to its string representation.
     * 
     * Returns a human-readable string name for the given MessageType enum value.
     * Useful for logging and debugging.
     * 
     * @param type The MessageType to convert.
     * 
     * @return Pointer to a null-terminated string describing the message type.
     */
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

    /**
     * @class VectorChar
     * @brief A wrapper class for std::vector<char> with multiple construction options.
     * 
     * VectorChar provides a flexible container for character data with various constructors
     * to accept different input types (C-strings, std::string, integers, vectors, etc.).
     * It simplifies the creation of message payloads from diverse data types.
     */
    class VectorChar
    {
    public:
        /**
         * @brief Default constructor creating an empty data container.
         */
        VectorChar() = default;

        /**
         * @brief Constructs from a C-string.
         * 
         * @param str Pointer to a null-terminated C-string.
         */
        VectorChar(const char *str)
            : m_data(str, str + std::strlen(str))
        {
        }

        /**
         * @brief Constructs from a std::string.
         * 
         * @param str Reference to a std::string.
         */
        VectorChar(const std::string &str)
            : m_data(str.begin(), str.end())
        {
        }

        /**
         * @brief Constructs from a single uint8_t value.
         * 
         * @param value The byte value to store.
         */
        VectorChar(uint8_t value)
            : m_data{static_cast<char>(value)}
        {
        }

        /**
         * @brief Constructs from a single char value.
         * 
         * @param value The character to store.
         */
        VectorChar(char value)
            : m_data{value}
        {
        }

        /**
         * @brief Constructs from a vector of chars (move semantics).
         * 
         * @param data The vector to move into this object.
         */
        VectorChar(const std::vector<char> &data)
            : m_data(std::move(data))
        {
        }

        /**
         * @brief Constructs from a vector of uint8_t values.
         * 
         * Converts each uint8_t value to a char.
         * 
         * @param data The vector of bytes to convert and store.
         */
        VectorChar(const std::vector<uint8_t> &data)
        {
            m_data.reserve(data.size());
            for (uint8_t byte : data)
            {
                m_data.push_back(static_cast<char>(byte));
            }
        }

        /**
         * @brief Deleted constructor for unsupported types.
         * 
         * Prevents implicit construction from unsupported types.
         */
        template <typename T>
        VectorChar(const T &) = delete;

        /**
         * @brief Moves the internal data vector.
         * 
         * @return rvalue reference to the internal vector.
         */
        std::vector<char> &&move() { return std::move(m_data); }
        
        /**
         * @brief Gets a reference to the internal data vector.
         * 
         * @return Mutable reference to the underlying vector.
         */
        std::vector<char> &get() { return m_data; }
        
        /**
         * @brief Gets a const reference to the internal data vector.
         * 
         * @return Const reference to the underlying vector.
         */
        const std::vector<char> &get() const { return m_data; }

    protected:
        std::vector<char> m_data;
    };

}
