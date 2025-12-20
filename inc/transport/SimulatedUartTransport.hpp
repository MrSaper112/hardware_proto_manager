#pragma once 

#include "ITransport.hpp"

namespace transport
{
	class SimulatedUartTransport : public ITransport {

	public:
		SimulatedUartTransport() = default;
		~SimulatedUartTransport() override = default;

	};
}