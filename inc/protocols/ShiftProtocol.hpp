#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace wm::protoc
{
	/**
	 * @class ShiftProtocol
	 * @brief Protocol implementation with character shift encoding.
	 * 
	 * ShiftProtocol implements a simple encoding/decoding scheme that shifts
	 * each character by a fixed amount before transmission and shifts back
	 * when receiving. This provides basic obfuscation and can help with
	 * byte value constraints (e.g., avoiding null bytes).
	 * 
	 * The shift amount is configurable during construction and remains
	 * constant for the lifetime of the protocol adapter.
	 * 
	 * @note This is a simple encoding scheme and should not be relied upon
	 * for security purposes. It's primarily useful for data transformation.
	 */
	class ShiftProtocol : public IProtocolAdapter
	{
	public:
		/**
		 * @brief Constructs a ShiftProtocol with a specified shift value.
		 * 
		 * @param charShift The number of positions to shift each character.
		 *                  Should be in range 0-255 for meaningful transformation.
		 */
		ShiftProtocol(uint16_t charShift) : charShift(charShift) {};

		/**
		 * @brief Destructor.
		 */
		~ShiftProtocol() override = default;

		/**
		 * @brief Encodes a message with character shift encoding.
		 * 
		 * Serializes the message and then shifts each byte by the configured
		 * shift amount.
		 * 
		 * @param mes The Message to encode.
		 * 
		 * @return The encoded message bytes (shifted).
		 */
		std::vector<char> encode(const Message &mes) override;
		
		/**
		 * @brief Decodes a shift-encoded message buffer.
		 * 
		 * Reverses the character shift transformation and deserializes
		 * the resulting bytes back into a Message object.
		 * 
		 * @param data Pointer to the shift-encoded message buffer.
		 * @param size The length of the buffer in bytes.
		 * 
		 * @return The decoded Message object.
		 */
		Message decode(const char *data, size_t size) override;
	private:

		/**
		 * @brief Encodes a single byte using the shift transformation.
		 * 
		 * @param byte The byte to encode.
		 * 
		 * @return The encoded byte.
		 */
		char encodeByte(const char byte);
		
		/**
		 * @brief Decodes a single byte using the reverse shift transformation.
		 * 
		 * @param byte The encoded byte to decode.
		 * 
		 * @return The decoded byte.
		 */
		char decodeByte(const char byte);

	private:
		/// @brief The shift amount for encoding/decoding.
		uint16_t charShift = 0;
	};
}
