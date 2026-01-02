#pragma once

#include "ITransport.hpp"
#include <thread>
#include <queue>
#include "messages/Message.hpp"

#ifdef _WIN32
	#include <windows.h>
#elif __unix__
	#include <termios.h>
	#include <unistd.h>
	#include <sys/ioctl.h>
#endif


#define RX_BUFF_SIZE 	 1024
#define TX_BUFF_SIZE 	 1024


namespace transport
{
	class UartTransport : public ITransport {
	public:
		UartTransport(const SerialConfig& config);
		~UartTransport() override;

		ErrorCode open() override;
		ErrorCode close() override;

	public:
		int sendMessage(const Message* mes);
		int available() const override;
		ErrorCode flush() override;

		SerialConfig get_config() const override 
		{
			return m_config;
		};


		int send(const Byte* data, size_t length) override;
		int receive(Byte* buffer, size_t length) override;
	private: 
		void startReciveThread()
		{
			m_thread = std::thread(&UartTransport::receiveThread, this);
		};

		void receiveThread();

	private: 
		std::queue<ByteBuffer> m_rx_queue;

		uint16_t thread_timeout = 1000;
		std::thread m_thread;

		Byte rx_buff[RX_BUFF_SIZE] = {0};
		Byte tx_buff[TX_BUFF_SIZE] = {0};

		std::vector<Message> mesRecieveQue;

#ifdef _WIN32
		HANDLE m_handle;
		ErrorCode configure_windows();
#elif __unix__
		int m_fd;
		ErrorCode configure_unix();
#endif
	};
}
