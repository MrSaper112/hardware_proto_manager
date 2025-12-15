#pragma once

#include "ITransport.hpp"
#include <thread>
#include <queue>


#ifdef _WIN32
	#include <windows.h>
#elif __unix__
	#include <termios.h>
	#include <unistd.h>
#endif


namespace transport
{
	class UartTransport : public ITransport {
	public:
		UartTransport(const SerialConfig& config);
		~UartTransport() override;

		ErrorCode open() override;
		ErrorCode close() override;
		bool is_open() const override;

		int send(const Byte* data, size_t length) override;
		int receive(Byte* buffer, size_t length, uint32_t timeout_ms) override;

		size_t available() const override;
		ErrorCode flush() override;

		SerialConfig get_config() const override 
		{
			return m_config;
		};

	private: 
		std::thread m_async_thread;
		std::atomic<bool> m_async_running;
		std::queue<ByteBuffer> m_rx_queue;


#ifdef _WIN32
		HANDLE m_handle;
		ErrorCode configure_windows();
#elif __unix__
		int m_fd;
		ErrorCode configure_unix();
#endif
	};
}
