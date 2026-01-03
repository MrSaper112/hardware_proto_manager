#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace protoc
{
	class PlainProtocol : public IProtocolAdapter
	{
	public:
		PlainProtocol() = default;
		~PlainProtocol() override = default;

		std::vector<char> encode(const Message &mes) override;

		Message decode(const char *data, size_t size) override;
	};
}
