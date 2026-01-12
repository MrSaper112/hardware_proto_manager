#include "devices/TestDevice.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace wm::devices;

TestDevice::TestDevice(transport::ITransport *transport, protoc::IProtocolAdapter *protocol)
    : IDevice(protocol, transport)

{
    transport->subscribeReceive([this](const Message &mes)
                                { this->onNotifyReceive(mes); });
}

void TestDevice::connect()
{
    auto status = m_transport->open();
    if (status == transport::ErrorCode::Success)
    {
        std::cout << TAG << "Connected successfully" << std::endl;
    }
    else
    {
        std::cout << TAG << "Failed to connect: " << static_cast<int>(status) << std::endl;
        throw std::runtime_error("Failed to connect, transport error code: " + std::to_string(static_cast<int>(status)));
    }
}

void TestDevice::disconnect()
{
    if (m_transport)
    {
        m_transport->close();
        std::cout << TAG << "Disconnected" << std::endl;
    }
}

bool TestDevice::sendTestMessage(uint32_t idx, MessageType type, const std::vector<char> &data)
{
    if (!m_protocol || !m_transport)
    {
        std::cout << TAG << "Error: Protocol or Transport not initialized" << std::endl;
        return false;
    }

    try
    {
        Message msg(idx, type, VectorChar(data));
        auto encoded = m_protocol->encode(msg);
        return sendRaw(encoded);
    }
    catch (const std::exception &e)
    {
        std::cout << TAG << "Error encoding message: " << e.what() << std::endl;
        return false;
    }
}

bool TestDevice::sendCommand(uint32_t idx, const std::vector<char> &data)
{
    return sendTestMessage(idx, MessageType::Command, data);
}

bool TestDevice::sendResponse(uint32_t idx, const std::vector<char> &data)
{
    return sendTestMessage(idx, MessageType::Response, data);
}

bool TestDevice::sendData(uint32_t idx, const std::vector<char> &data)
{
    return sendTestMessage(idx, MessageType::Data, data);
}

bool TestDevice::sendHeartbeat(uint32_t idx)
{
    std::vector<char> empty;
    return sendTestMessage(idx, MessageType::HeartBeat, empty);
}

bool TestDevice::sendRaw(const std::vector<char> &data)
{
    if (!m_transport)
    {
        throw std::runtime_error("Transport not initialized");
    }

    try
    {
        int bytes_sent = m_transport->send(data.data(), data.size());

        if (bytes_sent > 0)
        {
            std::cout << TAG << "Sent " << bytes_sent << " bytes: ";
            for (size_t i = 0; i < data.size() && i < 32; ++i)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned int>(data[i]) & 0xFF) << " " << std::dec;
            }
            if (data.size() > 32)
            {
                std::cout << "... (" << (data.size() - 32) << " more bytes)";
            }
            std::cout << std::endl;
            return true;
        }
        else
        {
            std::cout << TAG << "Send failed: " << bytes_sent << std::endl;
            return false;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << TAG << "Error sending data: " << e.what() << std::endl;
        return false;
    }
}
