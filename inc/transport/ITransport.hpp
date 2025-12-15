#pragma once
#include <cstdint>
#include <string>

#ifdef _WIN32
	#include <windows.h>
#elif __unix__
	#include <fcntl.h>
	#include <errno.h>
	#include <termios.h>
	#include <unistd.h>
#endif

#include "TransportTypes.hpp"

namespace transport {
	class ITransport {
	public:
		ITransport(const SerialConfig& config) : m_config(config) {};

		virtual ~ITransport() = default;
		virtual ErrorCode open() = 0;
		virtual ErrorCode close() = 0;
		virtual bool is_open() const = 0;
		
		ConnectionState get_state() const
		{
			return m_con_state;
		};

		virtual int send(const Byte* data, size_t length) = 0;
		virtual int send(const ByteBuffer& data) {
			return send(data.data(), data.size());
		}

		virtual int receive(Byte* buffer, size_t length, uint32_t timeout_ms) = 0;

		virtual ByteBuffer receive(size_t length, uint32_t timeout_ms) {
			ByteBuffer buffer(length);
			int bytes_read = receive(buffer.data(), length, timeout_ms);
			if (bytes_read > 0) {
				buffer.resize(bytes_read);
			}
			else {
				buffer.clear();
			}
			return buffer;
		}

		virtual size_t available() const = 0;
		virtual ErrorCode flush() = 0;
		virtual SerialConfig get_config() const = 0;
	protected:
		SerialConfig m_config;
		ConnectionState m_con_state{ ConnectionState::Closed };
	};
}