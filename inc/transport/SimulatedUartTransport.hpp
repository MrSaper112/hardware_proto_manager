#pragma once 

#include "ITransport.hpp"

class SimulatedUartTransport : public ITransport {
	
public:
	SimulatedUartTransport() = default;
	~SimulatedUartTransport() override = default;

	void connect() override;
	void disconnect() override;
	void sendData(const char* data, size_t size) override;
	void receiveData(char* buffer, size_t size) override;
}