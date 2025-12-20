#include "transport/TransportTypes.hpp"


#ifdef __unix__
namespace transport 
{
    speed_t baudrate_to_speed_t(BaudRate baudrate)
    {
        switch (baudrate) {
        case BaudRate::Baud300:    return B300;
        case BaudRate::Baud600:    return B600;
        case BaudRate::Baud1200:   return B1200;
        case BaudRate::Baud2400:   return B2400;
        case BaudRate::Baud4800:   return B4800;
        case BaudRate::Baud9600:   return B9600;
        case BaudRate::Baud19200:  return B19200;
        case BaudRate::Baud38400:  return B38400;
        case BaudRate::Baud57600:  return B57600;
        case BaudRate::Baud115200: return B115200;
        case BaudRate::Baud230400: return B230400;
        case BaudRate::Baud460800: return B460800;
        case BaudRate::Baud921600: return B921600;
        default:                   return B9600;
        }
    }
    unsigned int data_bits_to_csize(DataBits data_bits)
    {
        switch (data_bits) {
        case DataBits::Bits5: return CS5;
        case DataBits::Bits6: return CS6;
        case DataBits::Bits7: return CS7;
        case DataBits::Bits8: return CS8;
        default:              return CS8;
        }
    }

    unsigned int stop_bits_to_cstopb(StopBits stop_bits)
    {
        switch (stop_bits) {
        case StopBits::One:        return 0;
        case StopBits::OnePointFive:
        case StopBits::Two:        return CSTOPB;
        default:                    return 0;
        }
    }

    unsigned int parity_to_cparity(Parity parity)
    {
        switch (parity) {
        case Parity::None: return 0;
        case Parity::Odd:  return (PARENB | PARODD);
        case Parity::Even: return PARENB;
        default:           return 0;
        }
    }
}

#endif