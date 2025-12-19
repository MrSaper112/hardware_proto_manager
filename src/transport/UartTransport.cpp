#include "transport/UartTransport.hpp"
#include <iostream>

using namespace transport;

UartTransport::UartTransport(const SerialConfig &config): 
	ITransport(config), 
	m_async_running(false)
{
#ifdef _WIN32
	m_handle = INVALID_HANDLE_VALUE;
#elif __unix__
	m_fd = -1;
#endif
}

UartTransport::~UartTransport()
{
	close();
}

ErrorCode UartTransport::open()
{
	if (is_open()) {
		return ErrorCode::PortAlreadyOpen;
	}

	auto status = ErrorCode::Unknown;

#ifdef _WIN32
	status = configure_windows();
#elif __unix__
	status = configure_unix();
#endif

	if (status != ErrorCode::Success) {
		m_con_state = ConnectionState::Error;
		return status;
	}
	
	m_con_state = ConnectionState::Open;

	return status;
}

ErrorCode UartTransport::close()
{
	return ErrorCode::Success;
}

bool UartTransport::is_open() const
{
	return m_con_state == ConnectionState::Open;
}

int UartTransport::send(const Byte* data, size_t length)
{
	if (!is_open()) {
		throw PortException("Port not open", ErrorCode::PortNotOpen);
	}

	if (data == nullptr || length == 0) {
		return 0;
	}

#ifdef _WIN32

#else
	ssize_t bytes_written = ::write(m_fd, data, length);
	if (bytes_written < 0) {
		throw PortException("Failed to write to port", ErrorCode::OperationFailed);
	}
	return bytes_written;
#endif
}

int UartTransport::receive(Byte* buffer, size_t length, uint32_t timeout_ms)
{
	return static_cast<int>(length);
}

size_t UartTransport::available() const
{
	return 0;
}

ErrorCode UartTransport::flush()
{
	return ErrorCode::Success;
}

#ifdef _WIN32
ErrorCode transport::UartTransport::configure_windows()
{
	return ErrorCode::Success;
}

#elif __unix__

ErrorCode transport::UartTransport::configure_unix()
{
	struct termios options;
	std::cout << "Opening port: " << m_config.port << std::endl;
	m_fd = ::open(m_config.port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (m_fd == -1) {
		return ErrorCode::PortNotFound;
	}

	if (tcgetattr(m_fd, &options) != 0) {
		::close(m_fd);
		m_fd = -1;
		return ErrorCode::InvalidParameter;
	}

	speed_t speed = baudrate_to_speed_t(m_config.baudrate);

	cfsetospeed(&options, speed);
	cfsetispeed(&options, speed);

	options.c_cflag |= (data_bits_to_csize(m_config.databits) | CREAD | CLOCAL);

	options.c_cflag &= ~(parity_to_cparity(m_config.parity) | 
						 stop_bits_to_cstopb(m_config.stopbits) | CRTSCTS);
	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 1000;

	if (tcsetattr(m_fd, TCSANOW, &options) != 0) {
		::close(m_fd);
		m_fd = -1;
		return ErrorCode::InvalidParameter;
	}

	return ErrorCode::Success;
}

#endif
