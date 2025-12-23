#pragma once
#include <cstddef>
#include <ctime>
#include <cstdint>
#include "messages/Message.hpp"

namespace protoc
{
	template <typename PayloadType>
	struct Packet
	{
		
	};

	class IProtocolAdapter
	{
	public:
		IProtocolAdapter() = default;
		virtual ~IProtocolAdapter() = default;

		virtual VectorChar encodeData(const VectorChar &data) = 0;
		virtual VectorChar decodeData(const VectorChar &data) = 0;

		virtual VectorChar encodeString(const std::string &mes) = 0;
		virtual VectorChar decodeString(const std::string &mes) = 0;
	};
}
