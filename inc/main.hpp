#pragma once

/**
 * @namespace wm
 * @brief Main namespace for the Hardware Proto Manager project.
 * 
 * This namespace contains all core functionality for hardware device management,
 * including protocol adapters, transport layers, device implementations, and
 * message handling.
 */
namespace wm
{
    /// @brief Project name.
    constexpr const char *PROJECT_NAME = "HardwareProtoManager";
    /// @brief Project version.
    constexpr const char *PROJECT_VERSION = "1.0.0";
    /// @brief Project author.
    constexpr const char *PROJECT_AUTHOR = "Wiktor Moskwa";
    /// @brief Project license.
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