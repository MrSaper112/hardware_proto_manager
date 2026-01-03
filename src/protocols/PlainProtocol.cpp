#include "protocols/PlainProtocol.hpp"


using namespace protoc;

std::vector<char> PlainProtocol::encode(const Message &mes)
{
    auto serialized = mes.serialize();
    return serialized;
}

Message PlainProtocol::decode(const char *data, size_t size)
{
    return Message::deserialize(data, static_cast<uint8_t>(size));
}
