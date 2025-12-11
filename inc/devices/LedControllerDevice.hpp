#pragma once

#include "IDevice.hpp"

class LedControllerDevice : public IDevice {
public:
	LedControllerDevice() = default;
	~LedControllerDevice() override = default;

	void turnOn();
	void turnOff();
	void setBrightness(int level);
};