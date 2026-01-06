#pragma once

#include "IDevice.hpp"
#include <vector>
#include <cstdint>

namespace wm::devices
{
    class TestDevice : public IDevice
    {
    public:
        TestDevice(transport::ITransport *transport, protoc::IProtocolAdapter *protocol);

        ~TestDevice() = default;

        void connect() override;
        void disconnect() override;

        bool sendCommand(uint32_t idx, const std::vector<char> &data);
        bool sendResponse(uint32_t idx, const std::vector<char> &data);
        bool sendData(uint32_t idx, const std::vector<char> &data);
        bool sendHeartbeat(uint32_t idx);

        void onNotifyReceive(const Message &data) override
        {
            std::cout << TAG << "Received message notification:" << std::endl;
            data.print();
        }

        static constexpr const char *TAG = "[TestDevice] ";

    private:
        bool sendTestMessage(uint32_t idx, MessageType type, const std::vector<char> &data);
        bool sendRaw(const std::vector<char> &data);
    };
}