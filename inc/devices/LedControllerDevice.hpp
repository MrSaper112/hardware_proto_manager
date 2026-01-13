#pragma once

#include "IDevice.hpp"

namespace wm::devices
{
	/**
	 * @class LedPin
	 * @brief Represents a single LED pin configuration.
	 * 
	 * LedPin encapsulates the pin number and port information for controlling an LED.
	 * It provides methods to retrieve pin information and convert it to vector representation
	 * for communication protocols.
	 */
	class LedPin
	{
	public:
		/**
		 * @brief Constructs a LedPin with pin number and port identifier.
		 * 
		 * @param pin_number The GPIO pin number (0-255).
		 * @param port The port identifier character (e.g., 'A', 'B', 'C').
		 */
		LedPin(uint8_t pin_number, char port) : pin_number(pin_number), port(port) {}

		/**
		 * @brief Gets the port identifier.
		 * 
		 * @return The port character.
		 */
		char getPort() const { return port; }

		/**
		 * @brief Gets the pin number.
		 * 
		 * @return The GPIO pin number.
		 */
		uint8_t getPinNumber() const { return pin_number; }
		
		/**
		 * @brief Gets the pin number as a character.
		 * 
		 * @return The pin number cast as a char.
		 */
		char getPinChar() const { return static_cast<char>(pin_number); }

		/**
		 * @brief Converts pin information to a vector of characters.
		 * 
		 * The returned vector contains the pin character followed by the port character,
		 * suitable for protocol transmission.
		 * 
		 * @return Vector containing [pin_as_char, port].
		 */
		std::vector<char> toVectorChar() const
		{
			std::vector<char> vec;
			vec.push_back(getPinChar());
			vec.push_back(port);
			return vec;
		}

	private:
		uint8_t pin_number;
		char port;
	};

	/**
	 * @enum LedCommand
	 * @brief Commands for controlling LED behavior.
	 */
	enum LedCommand : char
	{
		TurnOn = 1,
		TurnOff = 2,
		SetBrightness = 3
	};

	/**
	 * @class LedControllerDevice
	 * @brief Device class for controlling LED hardware.
	 * 
	 * LedControllerDevice manages LED control through a specific GPIO pin and port.
	 * It inherits from IDevice and provides methods to control LED state and brightness.
	 * 
	 * @note Default pin configuration is pin 13 on port A.
	 */
	class LedControllerDevice : public IDevice
	{
	public:
		/**
		 * @brief Constructs an LedControllerDevice with default pin configuration.
		 * 
		 * @param transport Pointer to the transport layer implementation.
		 * @param protocol Pointer to the protocol adapter implementation.
		 */
		LedControllerDevice(transport::ITransport *transport, protoc::IProtocolAdapter *protocol);

		/**
		 * @brief Constructs an LedControllerDevice with custom pin configuration.
		 * 
		 * @param ledPin The LED pin configuration to use.
		 * @param transport Pointer to the transport layer implementation.
		 * @param protocol Pointer to the protocol adapter implementation.
		 */
		LedControllerDevice(const LedPin &ledPin, transport::ITransport *transport, protoc::IProtocolAdapter *protocol) : IDevice(protocol, transport), m_ledPin(ledPin) {};

		/**
		 * @brief Destructor.
		 */
		~LedControllerDevice() = default;

		/**
		 * @brief Establishes connection to the LED controller.
		 * 
		 * Implements the IDevice::connect() pure virtual method.
		 */
		void connect() override;
		
		/**
		 * @brief Closes connection to the LED controller.
		 * 
		 * Implements the IDevice::disconnect() pure virtual method.
		 */
		void disconnect() override;

		/**
		 * @brief Sends a command to turn the LED on.
		 */
		void turnOn();
		
		/**
		 * @brief Sends a command to turn the LED off.
		 */
		void turnOff();
		
		/**
		 * @brief Sends a command to set the LED brightness.
		 * 
		 * @param level The brightness level (0-255, where 0 is off and 255 is full brightness).
		 */
		void setBrightness(uint8_t level);

		/// @brief Logging tag for debug output.
		static constexpr const char *TAG = "[LedControllerDevice] ";

	private:
		LedPin m_ledPin{13, 'A'};
	};
}