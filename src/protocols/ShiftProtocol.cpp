#include "protocols/ShiftProtocol.hpp"

using namespace protoc;


std::vector<char> ShiftProtocol::encode(const Message &mes)
{
    std::vector<char> shifted;

    shifted.reserve(mes.data.get().size());

    for (char byte : mes.data.get())
    {
        shifted.push_back(encodeByte(byte));
    }

    Message encodedMessage = mes;
    encodedMessage.data = VectorChar(shifted);
    
    auto serialized = encodedMessage.serialize();

    return serialized;
}

char ShiftProtocol::encodeByte(const char byte)
{
    return static_cast<unsigned char>((byte + charShift) % 256);
}

char ShiftProtocol::decodeByte(const char byte)
{
    return static_cast<unsigned char>((byte - charShift + 256) % 256);
}

Message ShiftProtocol::decode(const char *data, size_t size)
{
    Message mes = Message::deserialize(data, size);

    std::vector<char> unshifted;
    unshifted.reserve(mes.data.get().size());

    for (char byte : mes.data.get())
    {
        unshifted.push_back(decodeByte(byte));
    }

    mes.data = VectorChar(unshifted);

    return mes;
}
