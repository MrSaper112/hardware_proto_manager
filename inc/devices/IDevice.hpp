#pragma once

#include <iostream>

#include "protocols/IProtocolAdapter.hpp"
#include "transport/ITransport.hpp"

class IDevice {
public:
	IDevice(protoc::IProtocolAdapter* protocol, transport::ITransport* transport)
		: m_protocol(protocol), m_transport(transport) 
		{
		}

	~IDevice() = default;

	virtual void connect() = 0;
	virtual void disconnect() = 0;

	virtual void onNotifyReceive(const Message& data) {};

	static constexpr const char* TAG = "[IDevice] ";
protected:
	protoc::IProtocolAdapter* m_protocol = nullptr;
	transport::ITransport* m_transport = nullptr;
};