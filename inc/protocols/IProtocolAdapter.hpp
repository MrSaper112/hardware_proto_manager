#pragma once
#include <cstddef>
#include <ctime>
#include <cstdint>

namespace protoc
{
	struct MessageHeader
	{
		std::time_t timestamp;
		uint8_t idx;
	};

	struct MessageData
	{
		uint8_t data_len;
		uint8_t *data[];
	};

	struct Message
	{
		MessageHeader identificator;
		MessageData data;
	};
	
	class IProtocolAdapter
	{
	public:
		IProtocolAdapter() = default;
		virtual ~IProtocolAdapter() = default;

		virtual void encodeData(const char *data, size_t size) = 0;
		virtual void decodeData(const char *data, size_t size) = 0;

		MessageHeader createMessageHeader(uint8_t idx = 0) 
		{	
			std::time_t t = std::time(0);
			MessageHeader mess = {
				.timestamp = t,
				.idx = idx,
			};

			return mess;
		};
	};
}
