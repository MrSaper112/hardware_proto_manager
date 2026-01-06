#pragma once

namespace wm
{
    constexpr const char *PROJECT_NAME = "HardwareProtoManager";
    constexpr const char *PROJECT_VERSION = "1.0.0";
    constexpr const char *PROJECT_AUTHOR = "Wiktor Moskwa";
    constexpr const char *PROJECT_LICENSE = "MIT License";
}

#include "transport/UartTransport.hpp"
#include "protocols/IProtocolAdapter.hpp"
#include "devices/IDevice.hpp"
#include "messages/Message.hpp"
#include "devices/TestDevice.hpp"
#include "devices/LedControllerDevice.hpp"
#include "protocols/PlainProtocol.hpp"
#include "protocols/ShiftProtocol.hpp"
#include <format>
#include <cstring>
#include <thread>
#include <chrono>
#include <iostream>