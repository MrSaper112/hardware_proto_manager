#pragma once
#include <cstddef>
#include <ctime>
#include <cstdint>
#include "messages/Message.hpp"

namespace protoc
{
	class IProtocolAdapter
	{
	public:
		IProtocolAdapter() = default;
		virtual ~IProtocolAdapter() = default;

		virtual void encodeData(const char *data, size_t size) = 0;
		virtual void decodeData(const char *data, size_t size) = 0;

	};
}
