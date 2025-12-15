#pragma once

#include "protocols/IProtocolAdapter.hpp"
#include "transport/ITransport.hpp"

class IDevice {
public:
	IDevice() = default;

	~IDevice() = default;

	virtual void connect() = 0;
	virtual void disconnect() = 0;
protected:
	IProtocolAdapter* m_protocol = nullptr;
	//ITransport* m_transport = nullptr;
};