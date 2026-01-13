#pragma once

#include <iostream>

#include "protocols/IProtocolAdapter.hpp"
#include "transport/ITransport.hpp"

namespace wm
{
	namespace devices
	{
		/**
		 * @class IDevice
		 * @brief Abstract base class for hardware device interfaces.
		 * 
		 * IDevice provides the foundation for all device implementations, managing the
		 * protocol adapter and transport layer communication. Derived classes must implement
		 * the connection lifecycle methods.
		 * 
		 * @note This is an abstract base class. Subclasses must implement pure virtual methods.
		 */
		class IDevice
		{
		public:
			/**
			 * @brief Constructs an IDevice with a protocol adapter and transport.
			 * 
			 * @param protocol Pointer to an IProtocolAdapter implementation for encoding/decoding messages.
			 * @param transport Pointer to an ITransport implementation for sending/receiving data.
			 * 
			 * @throws std::runtime_error If either protocol or transport pointer is null.
			 */
			IDevice(protoc::IProtocolAdapter *protocol, transport::ITransport *transport)
			{
				this->setProtocolAdapter(protocol);
				this->setTransport(transport);
			}

			/**
			 * @brief Sets the protocol adapter for message encoding/decoding.
			 * 
			 * @param protocol Pointer to an IProtocolAdapter implementation.
			 * 
			 * @throws std::runtime_error If protocol pointer is null.
			 */
			void setProtocolAdapter(protoc::IProtocolAdapter *protocol)
			{
				if (!protocol)
				{
					throw std::runtime_error("Protocol adapter is null");
				}

				m_protocol = protocol;
			}

			/**
			 * @brief Sets the transport layer for communication.
			 * 
			 * @param transport Pointer to an ITransport implementation.
			 * 
			 * @throws std::runtime_error If transport pointer is null.
			 */
			void setTransport(transport::ITransport *transport)
			{
				if (!transport)
				{
					throw std::runtime_error("Transport is null");
				}

				m_transport = transport;
			}

			/**
			 * @brief Destructor.
			 */
			~IDevice() = default;

			/**
			 * @brief Establishes a connection to the device.
			 * 
			 * Pure virtual method that must be implemented by derived classes
			 * to establish device-specific connections.
			 */
			virtual void connect() = 0;

			/**
			 * @brief Terminates the connection to the device.
			 * 
			 * Pure virtual method that must be implemented by derived classes
			 * to properly clean up device connections.
			 */
			virtual void disconnect() = 0;

			/**
			 * @brief Callback handler for received messages.
			 * 
			 * This method is called when the transport layer receives a message
			 * that needs to be processed by the device. Derived classes can override
			 * this to implement custom message handling.
			 * 
			 * @param data The received Message object.
			 */
			virtual void onNotifyReceive(const Message &data) {};

			/// @brief Logging tag for debug output.
			static constexpr const char *TAG = "[IDevice] ";

		protected:
			protoc::IProtocolAdapter *m_protocol = nullptr;
			transport::ITransport *m_transport = nullptr;
		};
	}
}