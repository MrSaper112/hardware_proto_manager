#pragma once
#include <cstddef>
#include <ctime>
#include <cstdint>
#include "messages/Message.hpp"
#include "messages/MessageTypes.hpp"
#include <cstring>
#include <concepts>

namespace protoc
{
	class IProtocolAdapter
	{
	public:
		IProtocolAdapter() = default;
		virtual ~IProtocolAdapter() = default;

		virtual VectorChar encode(const Message &mes) = 0;

		virtual Message decode(const char *data, size_t size) = 0;
		virtual Message decode(const VectorChar &data)
		{
			return this->decode(reinterpret_cast<const char *>(data.get().data()), data.get().size());
		};

		template <typename T>
		Message createCommand(const T &cmd)
		{
			return createMessage(MessageType::Command, cmd);
		}

		template <typename T>
		Message createResponse(const T &response)
		{
			return createMessage(MessageType::Response, response);
		}

		template <typename T>
		Message createDataMessage(const T &data)
		{
			return createMessage(MessageType::Data, data);
		}

		template <typename T>
		Message createError(const T &error)
		{
			return createMessage(MessageType::Error, error);
		}

		Message createHeartbeat()
		{
			return createMessage(MessageType::HeartBeat, VectorChar{});
		}

		uint32_t getMessageCounter() const { return m_mesCounter; }
		void resetCounter() { m_mesCounter = 0; }

	protected:
		template <typename T>
		Message createMessage(MessageType type, const T &data)
		{
			return Message(m_mesCounter++, type, VectorChar(data));
		}

		uint32_t m_mesCounter{0};	
	};

}
