#pragma once
#include <cstdint>
#include <string>
#include <thread> 


#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "TransportTypes.hpp"
#include <functional>
#include "../messages/Message.hpp"

namespace transport {
	class ITransport {
	public:
		ITransport(const SerialConfig& config) : m_config(config) {};

		virtual ~ITransport() = default;
		virtual ErrorCode open() = 0;
		virtual ErrorCode close() = 0;
		
		ConnectionState get_state() const
		{
			return m_con_state;
		};

	public:
		virtual int send(const char* data, size_t length) = 0;
		virtual int send(const ByteBuffer& data) {
			return send(data.data(), data.size());
		}

		virtual int receive(char* buffer, size_t length) = 0;

		virtual ByteBuffer receive(size_t length) {
			ByteBuffer buffer(length);
			int bytes_read = receive(buffer.data(), length);
			if (bytes_read > 0) {
				buffer.resize(bytes_read);
			}
			else {
				buffer.clear();
			}
			return buffer;
		}

		bool is_open() const
		{
			return m_con_state == ConnectionState::Open;
		}

		virtual int available() const = 0;
		virtual SerialConfig get_config() const = 0;

	public:
		void subscribeReceive(std::function<void(const Message&)> callback)
		{
			receive_callbacks.push_back(callback);
		}

		void notifyReceive(const Message& data)
		{
			for (const auto& callback : receive_callbacks)
			{
				callback(data);
			}
		}

	protected:
		std::vector<std::function<void(const Message&)>> receive_callbacks;
		SerialConfig m_config;
		ConnectionState m_con_state{ ConnectionState::Closed };
	};
}