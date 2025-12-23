#include "protocols/ShiftProtocol.hpp"

using namespace protoc;

VectorChar ShiftProtocol::encodeData(const VectorChar &data)
{
    VectorChar result;
    for (const char &byte : data)
    {
        result.push_back(encodeByte(byte));
    }
    return result;
}

VectorChar ShiftProtocol::decodeData(const VectorChar &data)
{
    VectorChar result;
    for (const char &byte : data)
    {
        result.push_back(decodeByte(byte));
    }
    return result;
}
