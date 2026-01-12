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

using namespace wm::messages;

namespace wm::transport {
	/**
	 * @class ITransport
	 * @brief Abstract base class for transport layer implementations.
	 * 
	 * ITransport provides the interface for sending and receiving data over various
	 * transport mediums. It handles the connection
	 * lifecycle and provides callback mechanisms for received messages.
	 * 
	 * Derived classes must implement the platform or medium-specific communication logic.
	 * 
	 * @note This is an abstract base class. Subclasses must implement pure virtual methods.
	 */
	class ITransport {
	public:
		/**
		 * @brief Constructs a transport with serial configuration.
		 * 
		 * @param config Reference to a SerialConfig struct containing transport settings.
		 */
		ITransport(const SerialConfig& config) : m_config(config) {};

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~ITransport() = default;
		
		/**
		 * @brief Opens the transport connection.
		 * 
		 * Pure virtual method that must be implemented by derived classes
		 * to establish the transport connection (e.g., open serial port).
		 * 
		 * @return ErrorCode indicating success or specific error condition.
		 */
		virtual ErrorCode open() = 0;
		
		/**
		 * @brief Closes the transport connection.
		 * 
		 * Pure virtual method that must be implemented by derived classes
		 * to safely close the transport connection.
		 * 
		 * @return ErrorCode indicating success or specific error condition.
		 */
		virtual ErrorCode close() = 0;
		
		/**
		 * @brief Gets the current connection state.
		 * 
		 * @return The current ConnectionState (Closed, Open, or Error).
		 */
		ConnectionState get_state() const
		{
			return m_con_state;
		};

	public:
		/**
		 * @brief Sends data over the transport.
		 * 
		 * Pure virtual method for sending raw byte data over the transport medium.
		 * 
		 * @param data Pointer to the data buffer to send.
		 * @param length The number of bytes to send.
		 * 
		 * @return Number of bytes successfully sent, or negative value on error.
		 */
		virtual int send(const char* data, size_t length) = 0;
		
		/**
		 * @brief Sends data from a ByteBuffer.
		 * 
		 * Convenience overload that sends a ByteBuffer.
		 * 
		 * @param data Reference to a ByteBuffer containing data to send.
		 * 
		 * @return Number of bytes successfully sent, or negative value on error.
		 */
		virtual int send(const ByteBuffer& data) {
			return send(data.data(), data.size());
		}

		/**
		 * @brief Receives data into a buffer.
		 * 
		 * Pure virtual method for receiving raw byte data from the transport medium.
		 * 
		 * @param buffer Pointer to the buffer where received data will be stored.
		 * @param length The maximum number of bytes to receive.
		 * 
		 * @return Number of bytes successfully received, or negative value on error.
		 */
		virtual int receive(char* buffer, size_t length) = 0;

		/**
		 * @brief Receives data and returns as a ByteBuffer.
		 * 
		 * Convenience overload that returns received data in a ByteBuffer.
		 * 
		 * @param length The maximum number of bytes to receive.
		 * 
		 * @return A ByteBuffer containing the received data.
		 */
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

		/**
		 * @brief Checks if the transport is currently open.
		 * 
		 * @return true if the connection is open, false otherwise.
		 */
		bool is_open() const
		{
			return m_con_state == ConnectionState::Open;
		}

		/**
		 * @brief Gets the number of bytes available to read.
		 * 
		 * Pure virtual method that must be implemented to return the number of
		 * bytes currently available in the receive buffer.
		 * 
		 * @return Number of available bytes.
		 */
		virtual int available() const = 0;
		
		/**
		 * @brief Gets the current serial configuration.
		 * 
		 * Pure virtual method that must be implemented to return the transport's
		 * current configuration.
		 * 
		 * @return A copy of the current SerialConfig.
		 */
		virtual SerialConfig get_config() const = 0;

	public:
		/**
		 * @brief Subscribes to receive notifications.
		 * 
		 * Registers a callback function that will be invoked whenever a message
		 * is received on the transport.
		 * 
		 * @param callback A function that takes a const Message& parameter.
		 */
		void subscribeReceive(std::function<void(const Message&)> callback)
		{
			receive_callbacks.push_back(callback);
		}

		/**
		 * @brief Notifies all subscribers of a received message.
		 * 
		 * Calls all registered receive callbacks with the provided message.
		 * This method is typically called by the receive mechanism (e.g., receive thread)
		 * when a complete message is available.
		 * 
		 * @param data The received Message to notify subscribers about.
		 */
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