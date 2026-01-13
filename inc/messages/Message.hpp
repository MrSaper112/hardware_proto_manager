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
    /**
     * @class Message
     * @brief Encapsulates message data with serialization and deserialization capabilities.
     * 
     * The Message class represents a protocol message containing an indetification, type, and optional
     * payload data. It provides methods to serialize messages for transmission and deserialize
     * received message data.
     * 
     * Message Structure (binary format):
     * - Byte 0: Length (includes type and index but not this byte)
     * - Byte 1: MessageType
     * - Bytes 2-5: Index (32-bit)
     * - Bytes 6+: Payload data
     * 
     * @note The maximum message size is 255 bytes.
     */
    class Message
    {
    public:
        /// @brief Message length (payload size + type + index).
        uint8_t len;
        /// @brief Message identifier for tracking and correlation.
        uint32_t idx;
        /// @brief The type of message (Command, Response, Data, etc.).
        MessageType mesType;
        /// @brief Message payload data.
        VectorChar data;

        /**
         * @brief Default constructor creating an empty message.
         * 
         * Initializes an undefined message with zero length and index.
         */
        Message() : len(0), idx(0), mesType(MessageType::Undefined) {};

        /**
         * @brief Constructs a message with index and type but no payload.
         * 
         * @param index The message identifier.
         * @param type The MessageType for this message.
         */
        Message(uint32_t index, MessageType type) : idx(index), mesType(type)
        {
            this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType));
        }

        /**
         * @brief Constructs a message with index, type, and payload data.
         * 
         * @tparam T The payload data type (must be compatible with VectorChar).
         * @param index The message identifier.
         * @param type The MessageType for this message.
         * @param payload The message payload data.
         */
        template <typename T>
        Message(uint32_t index, MessageType type, const T &payload) : idx(index), mesType(type), data(payload)
        {
            this->len = static_cast<uint8_t>(sizeof(idx) + sizeof(mesType) + data.get().size());
        }

        /**
         * @brief Serializes the message into a byte buffer for transmission.
         * 
         * Converts the message into binary format suitable for transmission over
         * the transport layer. The serialized format includes the length, type, index,
         * and payload data.
         * 
         * @return Vector of chars containing the serialized message bytes.
         * 
         * @see deserialize
         */
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

        /**
         * @brief Deserializes a message from a vector of bytes.
         * 
         * Parses a serialized message buffer and reconstructs the Message object.
         * 
         * @param buffer Vector of chars containing the serialized message bytes.
         * 
         * @return Reconstructed Message object.
         * 
         * @throws std::runtime_error If buffer is too small (< 6 bytes) or too large (> 255 bytes).
         * 
         * @see serialize
         */
        static Message deserialize(const std::vector<char> &buffer)
        {
            auto msg = deserialize(buffer.data(), buffer.size());
            return msg;
        }

        /**
         * @brief Deserializes a message from raw character buffer.
         * 
         * Parses a raw byte buffer and reconstructs the Message object. The buffer must
         * contain at least the message preamble (6 bytes: length, type, index).
         * 
         * @param rxBuff Pointer to the raw byte buffer.
         * @param buffLen The length of the buffer in bytes.
         * 
         * @return Reconstructed Message object.
         * 
         * @throws std::runtime_error If buffer is too small or too large.
         * 
         * @see serialize
         */
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

        /**
         * @brief Prints the message contents to standard output.
         * 
         * Displays the message structure including length, index, type, and payload data
         * in both hexadecimal and ASCII formats for debugging purposes.
         */
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

        /**
         * @brief Compares two messages for equality.
         * 
         * Two messages are considered equal if they have the same index and length.
         * 
         * @param mes The message to compare with.
         * 
         * @return true if messages have the same index and length, false otherwise.
         */
        bool operator==(const Message &mes)
        {
            return this->idx == mes.idx && this->len == mes.len;
        }

    private:
        /// @brief Size of message preamble (length + type + index) in bytes.
        static constexpr uint8_t m_preambleSize = sizeof(len) + sizeof(mesType) + sizeof(idx);
    };
}