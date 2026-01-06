#include "protocols/PlainProtocol.hpp"

using namespace wm::protoc;

std::vector<char> PlainProtocol::encode(const Message &mes)
{
    auto serialized = mes.serialize();
    return serialized;
}

Message PlainProtocol::decode(const char *data, size_t size)
{
    return Message::deserialize(data, size);
}
