#include "transport/UartTransport.hpp"
#include <iostream>
#include <asm-generic/ioctls.h>
#include <cstring>

using namespace transport;

UartTransport::UartTransport(const SerialConfig &config) : ITransport(config)
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
	if (is_open())
	{
		return ErrorCode::PortAlreadyOpen;
	}

	auto status = ErrorCode::Unknown;

#ifdef _WIN32
	status = configure_windows();
#elif __unix__
	status = configure_unix();
#endif

	if (status != ErrorCode::Success)
	{
		m_con_state = ConnectionState::Error;
		return status;
	}

	m_con_state = ConnectionState::Open;

	try
	{
		// this->startReciveThread();
	}
	catch (std::exception ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return status;
}

void UartTransport::receiveThread()
{
	int bytes_count = 0;

	while (is_open())
	{
		bytes_count = this->available();
		if (bytes_count > 0)
		{
			try
			{
				memset(&rx_buff, 0, RX_BUFF_SIZE);

				if (this->receive(rx_buff, bytes_count, 100) < 0)
				{
					std::cout << "Invalid read" << std::endl;
					continue;
				};

				std::cout << "Recived: ";
				// if (rx_buff[0] == START_BYTE && rx_buff[bytes_count + 1] == END_BYTE)
				// {
				// 	for (size_t i = 0; i < bytes_count; i++)
				// 	{
				// 		std::cout << rx_buff[i];
				// 	}
				// 	std::cout << std::endl;

				// 	Byte ack = {};
				// 	snprintf(&ack, 1, "%x", ACK_BYTE);
				// 	this->send(&ack, 1);
				// }
			}
			catch (const std::exception ex)
			{
				std::cout << ex.what() << std::endl;
			};

			usleep(thread_timeout * 1);
		}
	};
};

ErrorCode UartTransport::close()
{
	return ErrorCode::Success;
}

int UartTransport::sendMessage(const Message* mes)
{
	auto serialized = mes->serialize();

	int status = this->send(serialized.data(), serialized.size());

	return 0;
}

int UartTransport::send(const Byte *data, size_t length)
{
	if (!is_open())
	{
		throw PortException("Port not open", ErrorCode::PortNotOpen);
	}

	if (data == nullptr || length == 0)
	{
		return 0;
	}

#ifdef _WIN32

#else
	std::cout << "Sending: " << length << " bytes" << std::endl;

	ssize_t bytes_written = ::write(m_fd, data, length);
	if (bytes_written < 0)
	{
		throw PortException("Failed to write to port", ErrorCode::OperationFailed);
	}
	return bytes_written;
#endif
}

int UartTransport::receive(Byte *buffer, size_t length, uint32_t timeout_ms)
{
	return read(m_fd, buffer, length);
}

int UartTransport::available() const
{
	int bytes;

	int status = ioctl(m_fd, FIONREAD, &bytes);
	if (status < 0)
	{
		std::cout << "Cannot read available bytes" << std::endl;
		return 0;
	}

	return bytes;
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
	if (m_fd == -1)
	{
		return ErrorCode::PortNotFound;
	}

	if (tcgetattr(m_fd, &options) != 0)
	{
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

	if (tcsetattr(m_fd, TCSANOW, &options) != 0)
	{
		::close(m_fd);
		m_fd = -1;
		return ErrorCode::InvalidParameter;
	}

	return ErrorCode::Success;
}

#endif
