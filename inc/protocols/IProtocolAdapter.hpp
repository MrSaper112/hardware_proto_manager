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

		virtual VectorChar encode(const Message &mes) = 0;
		virtual VectorChar encode(MessageType type, const VectorChar &data) = 0;
		virtual VectorChar encode(MessageType type, const std::string &data) 
		{
			VectorChar vc(data.begin(), data.end());
			return this->encode(type, vc);
		};

		virtual Message decode(const transport::Byte *data, size_t size) = 0;
		virtual Message decode(const VectorChar &data)
		{
			this->decode(&data.at(0), data.size());
			return Message();
		};

		Message createCommand(const std::string &cmd);
		Message createResponse(const std::string &response);
		Message createDataMessage(const VectorChar &data);
		Message createError(const std::string &error);
		Message createHeartbeat();

		uint32_t getMessageCounter() const { return m_mesCounter; }
		void resetCounter() { m_mesCounter = 0; }

	protected:
		uint32_t m_mesCounter;
	};

}
