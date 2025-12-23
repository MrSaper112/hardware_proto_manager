#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace protoc
{
	class PlainProtocol : public IProtocolAdapter
	{
	public:
		PlainProtocol() = default;
		~PlainProtocol() override;


	};
}
