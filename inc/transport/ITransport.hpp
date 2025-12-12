#pragma once
#include <cstdint>
#include <string>
#include <utility>

class ITransport {
public:
	ITransport(std::string port, const uint32_t baudrate):
		m_portName(std::move(port)), m_baudRate(baudrate) {};

	virtual ~ITransport() = default;

	virtual void workLoop() = 0;
	virtual void connect() = 0;
	virtual void disconnect() = 0;
	virtual void sendData(const char* data, size_t size) = 0;
	virtual void receiveData(char* buffer, size_t size) = 0;

protected:
	std::string m_portName;
	uint32_t m_baudRate;
};