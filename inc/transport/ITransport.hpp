#pragma once

class IProtocolAdapter {
public:
	IProtocolAdapter() = default;
	~IProtocolAdapter() = default;
	virtual void connect() = 0;
	virtual void disconnect() = 0;

	virtual void sendData(const char* data, size_t size) = 0;
	virtual void receiveData(char* buffer, size_t size) = 0;
};