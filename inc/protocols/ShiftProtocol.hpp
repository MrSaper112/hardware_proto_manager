#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace protoc
{
	class ShiftProtocol : public IProtocolAdapter
	{
	public:
		ShiftProtocol(uint16_t charShift) : charShift(charShift) {};

		~ShiftProtocol() override = default;

		std::vector<char> encode(const Message &mes) override;
		Message decode(const char *data, size_t size) override;
	private:

		char encodeByte(const char byte);
		char decodeByte(const char byte);

	private:
		uint16_t charShift = 0;
	};
}
