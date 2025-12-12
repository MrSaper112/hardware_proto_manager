#pragma once

#include "ITransport.hpp"

class UartTransport : public ITransport {
public:
	UartTransport(const std::string& port, const uint32_t baudrate): ITransport(port, baudrate) {};

	void connect() override;
	void disconnect() override;
	void sendData(const char* data, size_t size) override;
	void receiveData(char* buffer, size_t size) override;
};