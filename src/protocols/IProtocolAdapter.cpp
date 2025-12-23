#include "protocols/IProtocolAdapter.hpp"

using namespace protoc;

Message IProtocolAdapter::createCommand(const std::string &cmd)
{
    return Message(this->m_mesCounter++, MessageType::Command, cmd);
}

Message IProtocolAdapter::createResponse(const std::string &response)
{
    return Message(this->m_mesCounter++, MessageType::Response, response);
}

Message IProtocolAdapter::createDataMessage(const VectorChar &data)
{
    return Message(this->m_mesCounter++, MessageType::Data, data);
}

Message IProtocolAdapter::createError(const std::string &error)
{
    return Message(this->m_mesCounter++, MessageType::Error, error);
}

Message IProtocolAdapter::createHeartbeat()
{
    return Message(this->m_mesCounter++, MessageType::HeartBeat);
}
