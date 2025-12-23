#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace protoc
{
	class ShiftProtocol : public IProtocolAdapter
	{
	public:
		ShiftProtocol(uint16_t charShift) : charShift(charShift) {};

		~ShiftProtocol() override;

		VectorChar encodeData(const VectorChar &mes);
		VectorChar decodeData(const VectorChar &mes);

    char encodeByte(const char byte) {
        return static_cast<unsigned char>((byte + charShift) % 256);
    }

    char decodeByte(const char byte) {
        return static_cast<unsigned char>((byte - charShift + 256) % 256);
    }

	private:
		uint16_t charShift = 0;
	};
}
