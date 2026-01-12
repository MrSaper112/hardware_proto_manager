#pragma once
#include <cstddef>
#include <ctime>
#include <cstdint>
#include "messages/Message.hpp"
#include "messages/MessageTypes.hpp"
#include <cstring>
#include <concepts>

using namespace wm::messages;

namespace wm::protoc
{
	/**
	 * @class IProtocolAdapter
	 * @brief Abstract base class for protocol encoding/decoding adapters.
	 * 
	 * IProtocolAdapter defines the interface for protocol implementations that encode
	 * messages for transmission and decode received data back into Message objects.
	 * It also provides helper methods for creating different types of messages with
	 * automatic counter management.
	 * 
	 * Derived classes must implement the encode() and decode() methods to define
	 * the specific protocol encoding/decoding logic.
	 * 
	 * @note This is an abstract base class. Subclasses must implement pure virtual methods.
	 */
	class IProtocolAdapter
	{
	public:
		/**
		 * @brief Default constructor.
		 */
		IProtocolAdapter() = default;
		
		/**
		 * @brief Virtual destructor.
		 */
		virtual ~IProtocolAdapter() = default;

		/**
		 * @brief Encodes a Message into a byte buffer for transmission.
		 * 
		 * Pure virtual method that must be implemented by derived classes to convert
		 * a Message object into the protocol-specific binary format.
		 * 
		 * @param mes The Message object to encode.
		 * 
		 * @return Vector of chars containing the encoded message bytes.
		 */
		virtual std::vector<char> encode(const Message &mes) = 0;
		
		/**
		 * @brief Decodes a byte buffer into a Message object.
		 * 
		 * Pure virtual method that must be implemented by derived classes to parse
		 * protocol-specific binary data and reconstruct a Message object.
		 * 
		 * @param data Pointer to the raw byte buffer to decode.
		 * @param size The length of the buffer in bytes.
		 * 
		 * @return Reconstructed Message object.
		 */
		virtual Message decode(const char *data, size_t size) = 0;

		/**
		 * @brief Decodes a VectorChar buffer into a Message object.
		 * 
		 * Convenience overload that decodes from a VectorChar object.
		 * 
		 * @param data Reference to a VectorChar containing the data to decode.
		 * 
		 * @return Reconstructed Message object.
		 */
		virtual Message decode(const VectorChar &data)
		{
			return this->decode(reinterpret_cast<const char *>(data.get().data()), data.get().size());
		};

		/**
		 * @brief Creates a command message.
		 * 
		 * @tparam T The payload data type.
		 * @param cmd The command payload data.
		 * 
		 * @return A new Message of type Command.
		 */
		template <typename T>
		Message createCommand(const T &cmd)
		{
			return createMessage(MessageType::Command, cmd);
		}

		/**
		 * @brief Creates a response message.
		 * 
		 * @tparam T The payload data type.
		 * @param response The response payload data.
		 * 
		 * @return A new Message of type Response.
		 */
		template <typename T>
		Message createResponse(const T &response)
		{
			return createMessage(MessageType::Response, response);
		}

		/**
		 * @brief Creates a data message.
		 * 
		 * @tparam T The payload data type.
		 * @param data The data payload.
		 * 
		 * @return A new Message of type Data.
		 */
		template <typename T>
		Message createDataMessage(const T &data)
		{
			return createMessage(MessageType::Data, data);
		}

		/**
		 * @brief Creates an error message.
		 * 
		 * @tparam T The payload data type.
		 * @param error The error payload data.
		 * 
		 * @return A new Message of type Error.
		 */
		template <typename T>
		Message createError(const T &error)
		{
			return createMessage(MessageType::Error, error);
		}

		/**
		 * @brief Creates a heartbeat/keepalive message.
		 * 
		 * Constructs a new message of type HeartBeat with no payload.
		 * 
		 * @return A new Message of type HeartBeat.
		 */
		Message createHeartbeat()
		{
			return createMessage(MessageType::HeartBeat, VectorChar{});
		}

		/**
		 * @brief Gets the current message counter value.
		 * 
		 * Returns the current counter that will be used for the next message.
		 * 
		 * @return The current message counter value.
		 */
		uint32_t getMessageCounter() const { return m_mesCounter; }
		
		/**
		 * @brief Resets the message counter to zero.
		 * 
		 * Useful for resetting the message sequence after a connection reset
		 * or protocol restart.
		 */
		void resetCounter() { m_mesCounter = 0; }

	protected:
		/**
		 * @brief Internal helper to create a message with automatic counter management.
		 * 
		 * Creates a new message with the specified type and payload, using the
		 * current counter value. The counter is incremented after the message is created.
		 * 
		 * @tparam T The payload data type.
		 * @param type The MessageType for the new message.
		 * @param data The message payload.
		 * 
		 * @return A new Message with the current counter and incremented counter state.
		 */
		template <typename T>
		Message createMessage(MessageType type, const T &data)
		{
			return Message(m_mesCounter++, type, VectorChar(data));
		}

		/// @brief Internal counter for tracking message sequences.
		uint32_t m_mesCounter{0};	
	};

}
