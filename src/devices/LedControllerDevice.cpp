#include "devices/LedControllerDevice.hpp"

using namespace wm::devices;

LedControllerDevice::LedControllerDevice(transport::ITransport *transport, protoc::IProtocolAdapter *protocol)
    : IDevice(protocol, transport) {}

void LedControllerDevice::connect()
{
    if (m_transport == nullptr)
    {
        throw std::runtime_error("Transport not initialized");
    }

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

void LedControllerDevice::disconnect()
{
    if (m_transport)
    {
        m_transport->close();
        std::cout << TAG << "Disconnected" << std::endl;
    }
}

void LedControllerDevice::turnOn()
{
    try
    {
        // Construct command data: [PIN_STATE, PIN_NUMBER, PORT]
        std::vector<char> cmdData = {LedCommand::TurnOn, m_ledPin.getPinChar(), m_ledPin.getPort()};

        auto cmd = m_protocol->createCommand(cmdData);
        auto encoded = m_protocol->encode(cmd);

        m_transport->send(encoded.data(), encoded.size());
        std::cout << TAG << "Turn On command sent" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << TAG << "Error sending Turn On command: " << e.what() << std::endl;
    }
}

void LedControllerDevice::turnOff()
{
    try
    {
        std::vector<char> cmdData = {LedCommand::TurnOff, m_ledPin.getPinChar(), m_ledPin.getPort()};

        auto cmd = m_protocol->createCommand(cmdData);
        auto encoded = m_protocol->encode(cmd);

        m_transport->send(encoded.data(), encoded.size());
        std::cout << TAG << "Turn Off command sent" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << TAG << "Error sending Turn Off command: " << e.what() << std::endl;
    }
}

void LedControllerDevice::setBrightness(uint8_t level)
{
    try
    {
        std::vector<char> cmdData = {LedCommand::SetBrightness, m_ledPin.getPinChar(), m_ledPin.getPort(), static_cast<char>(level)};

        auto cmd = m_protocol->createCommand(cmdData);
        auto encoded = m_protocol->encode(cmd);
        m_transport->send(encoded.data(), encoded.size());
        std::cout << TAG << "Set Brightness command sent with level: " << level << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << TAG << "Error sending Set Brightness command: " << e.what() << std::endl;
    }
}
