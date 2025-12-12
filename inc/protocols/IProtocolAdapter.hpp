#pragma once
#include <cstddef>

class IProtocolAdapter {
public:
	IProtocolAdapter() = default;
	~IProtocolAdapter() = default;

	virtual void encodeData(const char* data, size_t size) = 0;
	virtual void decodeData(const char* data, size_t size) = 0;
};