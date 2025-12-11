#pragma once

#include "IProtocolAdapter.hpp"

class PlanProtocolAdapter : public IProtocolAdapter {
public:
	PlanProtocolAdapter() = default;
	~PlanProtocolAdapter() override = default;
	
	void parseMessage(const char* message) override;
	const char* createMessage(const char* command, const char* payload) override;
};