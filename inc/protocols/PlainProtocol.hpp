#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace wm::protoc
{
	/**
	 * @class PlainProtocol
	 * @brief Plain text protocol implementation with no encoding/decoding.
	 * 
	 * PlainProtocol is a simple protocol adapter that passes messages through
	 * without any additional encoding or decoding. It serializes/deserializes
	 * messages using the Message class's built-in serialization methods.
	 * 
	 * @note This implementation provides minimal overhead but offers no
	 * additional encoding benefits such as error correction or compression.
	 */
	class PlainProtocol : public IProtocolAdapter
	{
	public:
		/**
		 * @brief Constructs a PlainProtocol adapter.
		 */
		PlainProtocol() = default;
		
		/**
		 * @brief Destructor.
		 */
		~PlainProtocol() override = default;

		/**
		 * @brief Encodes a message using plain binary format.
		 * 
		 * Serializes the message directly to its binary representation
		 * without any additional encoding.
		 * 
		 * @param mes The Message to encode.
		 * 
		 * @return The serialized message bytes.
		 */
		std::vector<char> encode(const Message &mes) override;

		/**
		 * @brief Decodes a plain binary message buffer.
		 * 
		 * Deserializes raw bytes back into a Message object.
		 * 
		 * @param data Pointer to the binary message buffer.
		 * @param size The length of the buffer in bytes.
		 * 
		 * @return The decoded Message object.
		 */
		Message decode(const char *data, size_t size) override;
	};
}
