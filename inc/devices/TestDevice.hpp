#pragma once

#include "IDevice.hpp"
#include <vector>
#include <cstdint>

namespace wm::devices
{
    /**
     * @class TestDevice
     * @brief Test device implementation for debugging and protocol testing.
     * 
     * TestDevice is a utility device designed for testing and debugging the communication
     * protocol and transport layers. It allows sending various types of test messages
     * (commands, responses, data, heartbeats) and handles received messages with logging.
     * 
     * @note This device is primarily intended for development and testing purposes.
     */
    class TestDevice : public IDevice
    {
    public:
        /**
         * @brief Constructs a TestDevice.
         * 
         * @param transport Pointer to the transport layer implementation.
         * @param protocol Pointer to the protocol adapter implementation.
         */
        TestDevice(transport::ITransport *transport, protoc::IProtocolAdapter *protocol);

        /**
         * @brief Destructor.
         */
        ~TestDevice() = default;

        /**
         * @brief Establishes connection to the test device.
         * 
         * Implements the IDevice::connect() pure virtual method.
         */
        void connect() override;
        
        /**
         * @brief Closes connection to the test device.
         * 
         * Implements the IDevice::disconnect() pure virtual method.
         */
        void disconnect() override;

        /**
         * @brief Sends a command message.
         * 
         * Creates and sends a message of type Command with the specified data.
         * 
         * @param idx The message identifier.
         * @param data The command payload data.
         * 
         * @return true if the message was sent successfully, false otherwise.
         */
        bool sendCommand(uint32_t idx, const std::vector<char> &data);
        
        /**
         * @brief Sends a response message.
         * 
         * Creates and sends a message of type Response with the specified data.
         * 
         * @param idx The message index/identifier.
         * @param data The response payload data.
         * 
         * @return true if the message was sent successfully, false otherwise.
         */
        bool sendResponse(uint32_t idx, const std::vector<char> &data);
        
        /**
         * @brief Sends a data message.
         * 
         * Creates and sends a message of type Data with the specified payload.
         * 
         * @param idx The message index/identifier.
         * @param data The data payload.
         * 
         * @return true if the message was sent successfully, false otherwise.
         */
        bool sendData(uint32_t idx, const std::vector<char> &data);
        
        /**
         * @brief Sends a heartbeat message.
         * 
         * Creates and sends a heartbeat message with no payload data.
         * 
         * @param idx The message index/identifier.
         * 
         * @return true if the message was sent successfully, false otherwise.
         */
        bool sendHeartbeat(uint32_t idx);

        /**
         * @brief Callback handler for received messages.
         * 
         * Overrides IDevice::onNotifyReceive() to print received message details.
         * This is useful for debugging and testing.
         * 
         * @param data The received Message object.
         */
        void onNotifyReceive(const Message &data) override
        {
            std::cout << TAG << "Received message notification:" << std::endl;
            data.print();
        }

        /// @brief Logging tag for debug output.
        static constexpr const char *TAG = "[TestDevice] ";

    private:
        /**
         * @brief Internal helper to send test messages with specified type.
         * 
         * @param idx The message identifier.
         * @param type The MessageType to send.
         * @param data The message payload.
         * 
         * @return true if the message was sent successfully, false otherwise.
         */
        bool sendTestMessage(uint32_t idx, MessageType type, const std::vector<char> &data);
        
        /**
         * @brief Sends raw serialized message data.
         * 
         * @param data The raw message bytes to send.
         * 
         * @return true if the data was sent successfully, false otherwise.
         */
        bool sendRaw(const std::vector<char> &data);
    };
}