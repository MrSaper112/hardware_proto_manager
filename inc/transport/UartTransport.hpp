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
	/**
	 * @class UartTransport
	 * @brief UART/Serial transport implementation.
	 * 
	 * UartTransport provides a concrete implementation of the ITransport interface
	 * for UART/serial communication. It handles opening/closing serial ports,
	 * sending and receiving data, and managing a receive thread for asynchronous
	 * message processing.
	 */
	class UartTransport : public ITransport
	{
	public:
		/**
		 * @brief Constructs a UartTransport with serial configuration.
		 * 
		 * @param config Reference to a SerialConfig containing port and communication settings.
		 */
		UartTransport(const SerialConfig &config);
		/**
		 * @brief Destructor that closes the connection if open.
		 */
		~UartTransport() override;

		/**
		 * @brief Opens the serial port and starts the receive thread.
		 * 
		 * @return ErrorCode indicating success or the specific error.
		 */
		ErrorCode open() override;
		/**
		 * @brief Closes the serial port and stops the receive thread.
		 * 
		 * @return ErrorCode indicating success or the specific error.
		 */
		ErrorCode close() override;

	public:
		/**
		 * @brief Sends a complete message over the serial port.
		 * 
		 * @param mes Pointer to the Message to send.
		 * 
		 * @return Number of bytes sent, or negative value on error.
		 */
		int sendMessage(const Message *mes);
		/**
		 * @brief Gets the number of bytes available to read from the port.
		 * 
		 * @return Number of bytes available.
		 */
		int available() const override;

		/**
		 * @brief Gets the current serial configuration.
		 * 
		 * @return A copy of the SerialConfig in use.
		 */
		SerialConfig get_config() const override
		{
			return m_config;
		};

		/**
		 * @brief Sends raw data over the serial port.
		 * 
		 * @param data Pointer to the data buffer to send.
		 * @param length Number of bytes to send.
		 * 
		 * @return Number of bytes successfully sent.
		 */
		int send(const char *data, size_t length) override;
		/**
		 * @brief Receives raw data from the serial port.
		 * 
		 * @param buffer Pointer to the buffer where received data will be stored.
		 * @param length Maximum number of bytes to receive.
		 * 
		 * @return Number of bytes received.
		 */
		int receive(char *buffer, size_t length) override;

	private:
		/**
		 * @brief Starts the asynchronous receive thread.
		 */
		void startReceiveThread()
		{
			m_thread = std::thread(&UartTransport::receiveThread, this);
		};

		/**
		 * @brief Main loop for the receive thread.
		 * 
		 * Continuously monitors the serial port for incoming data,
		 * deserializes messages, and notifies subscribers.
		 */
		void receiveThread();

	private:
		/// @brief Queue for buffering received messages.
		std::queue<ByteBuffer> m_rx_queue;

		/// @brief Timeout for receive thread operations in milliseconds.
		uint16_t thread_timeout = 1;
		/// @brief The receive thread object.
		std::thread m_thread;

		/// @brief Receive buffer (RX_BUFF_SIZE bytes).
		char rx_buff[RX_BUFF_SIZE] = {0};
		/// @brief Transmit buffer (TX_BUFF_SIZE bytes).
		char tx_buff[TX_BUFF_SIZE] = {0};

		/// @brief Mutex protecting the receive queue.
		std::mutex mtxReceive;

		/// @brief File descriptor for the serial port (-1 if not open).
		int m_fd{-1};
		/**
		 * @brief Configures the UNIX serial port with termios settings.
		 * 
		 * @return ErrorCode indicating success or the specific error.
		 */
		ErrorCode configure_unix();
	};
}
