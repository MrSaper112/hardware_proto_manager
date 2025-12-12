#pragma once

#include "IDevice.hpp"

class BootloaderDevice : public IDevice {
public:
	BootloaderDevice() = default;
	~BootloaderDevice() override = default;
};