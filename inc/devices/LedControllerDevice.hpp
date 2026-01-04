#pragma once

#include "IDevice.hpp"

class LedPin
{
public:
	LedPin(uint8_t pin_number, char port) : pin_number(pin_number), port(port) {}

	char getPort() const { return port; }
	
	uint8_t getPinNumber() const { return pin_number; }
	char getPinChar() const { return static_cast<char>(pin_number); }



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

enum LedCommand : char {
	TurnOn = 1,
	TurnOff = 2,
	SetBrightness = 3
	
};

class LedControllerDevice : public IDevice {
public:
	LedControllerDevice(transport::ITransport *transport, protoc::IProtocolAdapter *protocol);
	
	LedControllerDevice(const LedPin& ledPin, transport::ITransport *transport, protoc::IProtocolAdapter *protocol):
		IDevice(protocol, transport), m_ledPin(ledPin) {};

    ~LedControllerDevice() = default;

    void connect() override;
    void disconnect() override;

	void turnOn();
	void turnOff();
	void setBrightness(uint8_t level);

	static constexpr const char* TAG = "[LedControllerDevice] ";
private:
	LedPin m_ledPin{13, 'A'};
};