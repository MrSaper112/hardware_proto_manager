#pragma once

#include "IProtocolAdapter.hpp"
#include <cstdint>

namespace protoc {
class PlanProtocolAdapter : public IProtocolAdapter {
public:
	PlanProtocolAdapter() = default;
	~PlanProtocolAdapter() override;

	void encodeData(const char *data, size_t size) override;
	void decodeData(const char *data, size_t size) override;

};

}
