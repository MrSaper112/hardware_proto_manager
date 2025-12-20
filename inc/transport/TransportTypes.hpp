#pragma once

#include <cstdint>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <windows.h>    
#elif __unix__
    #include <fcntl.h>
    #include <errno.h>
    #include <termios.h>
    #include <unistd.h>
#endif

namespace transport {
    using Byte = char;
    using ByteBuffer = std::vector<Byte>;
    using PortName = std::string;
    using Timestamp = uint64_t;

    enum class BaudRate : uint32_t {
        Baud300 = 300,
        Baud600 = 600,
        Baud1200 = 1200,
        Baud2400 = 2400,
        Baud4800 = 4800,
        Baud9600 = 9600,
        Baud14400 = 14400,
        Baud19200 = 19200,
        Baud28800 = 28800,
        Baud38400 = 38400,
        Baud57600 = 57600,
        Baud115200 = 115200,
        Baud230400 = 230400,
        Baud460800 = 460800,
        Baud921600 = 921600,
    };

    enum class DataBits {
        Bits5 = 5,
        Bits6 = 6,
        Bits7 = 7,
        Bits8 = 8,
    };

    enum class StopBits {
        One = 1,
        OnePointFive = 3,
        Two = 2,
    };

    enum class Parity {
        None = 0,
        Odd = 1,
        Even = 2,
    };

    enum class TransportType {
        UART,
        USB,
        TCP,
        BLE,
        Custom,
    };

    enum class ErrorCode {
        Success = 0,
        PortNotFound = 1,
        PortAlreadyOpen = 2,
        PortNotOpen = 3,
        InvalidParameter = 4,
        OperationTimeout = 5,
        HardwareError = 6,
        InvalidFrame = 7,
        ChecksumError = 8,
        BufferOverflow = 9,
        OperationFailed = 10,
        Unknown = 255,
    };


    enum class EventType {
        PortOpened,
        PortClosed,
        DataReceived,
        DataSent,
        Error,
        Connected,
        Disconnected,
        StateChanged,
        Custom,
    };


    enum class ConnectionState {
        Closed = 0,
        Open = 1,
        Error = 2,
    };

    struct SerialConfig {
		PortName port = "";
        BaudRate baudrate = BaudRate::Baud115200;
        DataBits databits = DataBits::Bits8;
        StopBits stopbits = StopBits::One;
        Parity parity = Parity::None;
        uint32_t read_timeout_ms = 1000;
        uint32_t write_timeout_ms = 1000;
        size_t rx_buffer_size = 4096;
        size_t tx_buffer_size = 4096;
    };

    struct PortInfo {
        PortName port;
        std::string description;
        std::string hardware_id;
        TransportType type = TransportType::UART;
    };


    class TransportException : public std::exception {
    protected:
        std::string message_;
        ErrorCode error_code_;

    public:
        TransportException(const std::string& msg, ErrorCode code = ErrorCode::Unknown)
            : message_(msg), error_code_(code) {
        }

        virtual ~TransportException() = default;

        const char* what() const noexcept override {
            return message_.c_str();
        }

        ErrorCode get_error_code() const { return error_code_; }
    };

    class PortException : public TransportException {
    public:
        PortException(const std::string& msg, ErrorCode code)
            : TransportException(msg, code) {
        }
    };


    class TimeoutException : public TransportException {
    public:
        TimeoutException(const std::string& msg = "Operation timeout")
            : TransportException(msg, ErrorCode::OperationTimeout) {
        }
    };


#ifdef _WIN32

#elif __unix__
    speed_t baudrate_to_speed_t(BaudRate baudrate);
    unsigned int data_bits_to_csize(DataBits data_bits);
    unsigned int stop_bits_to_cstopb(StopBits stop_bits);
    unsigned int parity_to_cparity(Parity parity);

#endif
} // namespace transport
