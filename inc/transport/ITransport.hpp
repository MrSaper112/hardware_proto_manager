#pragma once
#include <string>

class ITransport {
public:
	ITransport() = default;
	~ITransport() = default;

	virtual void connect() = 0;
	virtual void disconnect() = 0;
	virtual void sendData(const char* data, size_t size) = 0;
	virtual void receiveData(char* buffer, size_t size) = 0;

protected:
	std::string m_portName;
	int m_baudRate;
};