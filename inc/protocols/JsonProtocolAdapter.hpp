#pragma once

#include "IProtocolAdapter.hpp"

namespace protoc
{
	class JsonProtocolAdapter : public IProtocolAdapter
	{
	public:
		JsonProtocolAdapter() = default;
		~JsonProtocolAdapter() override = default;

		void parseMessage(const char *message) override;
		const char *createMessage(const char *command, const char *payload) override;
	};

}