#pragma once

#include "ITransport.hpp"
#include <thread>
#include <queue>
#include "messages/Message.hpp"


#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <mutex>

#define RX_BUFF_SIZE 1024
#define TX_BUFF_SIZE 1024

namespace wm::transport
{
	class UartTransport : public ITransport
	{
	public:
		UartTransport(const SerialConfig &config);
		~UartTransport() override;

		ErrorCode open() override;
		ErrorCode close() override;

	public:
		int sendMessage(const Message *mes);
		int available() const override;

		SerialConfig get_config() const override
		{
			return m_config;
		};

		int send(const char *data, size_t length) override;
		int receive(char *buffer, size_t length) override;

	private:
		void startReceiveThread()
		{
			m_thread = std::thread(&UartTransport::receiveThread, this);
		};

		void receiveThread();

	private:
		std::queue<ByteBuffer> m_rx_queue;

		uint16_t thread_timeout = 1;
		std::thread m_thread;

		char rx_buff[RX_BUFF_SIZE] = {0};
		char tx_buff[TX_BUFF_SIZE] = {0};

		std::mutex mtxReceive;

		int m_fd{-1};
		ErrorCode configure_unix();
	};
}
