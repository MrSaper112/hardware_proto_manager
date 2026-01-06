#include "transport/UartTransport.hpp"
#include <iostream>
#include <asm-generic/ioctls.h>
#include <cstring>
#include <thread>

using namespace transport;

UartTransport::UartTransport(const SerialConfig &config) : ITransport(config)
{
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

	status = configure_unix();

	if (status != ErrorCode::Success)
	{
		m_con_state = ConnectionState::Error;
		return status;
	}

	m_con_state = ConnectionState::Open;

	try
	{
		this->startReceiveThread();
	}
	catch (const std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return status;
}

void UartTransport::receiveThread()
{
	int bytes_count = 0;
	std::cout << "Starting main receive thread" << std::endl;
	while (is_open())
	{
		bytes_count = this->available();
		if (bytes_count <= 0)
		{
			continue;
		}

		size_t bytes_read = this->receive(rx_buff, 1);
		if (bytes_read != 1)
		{
			std::cout << "Failed to read length byte" << std::endl;
			continue;
		}

		uint8_t expected_len = rx_buff[0];
		if (expected_len == 0 || expected_len > (RX_BUFF_SIZE - 1))
		{
			std::cout << "Invalid length: " << static_cast<int>(expected_len) << std::endl;
			continue;
		}

		auto start_time = std::chrono::steady_clock::now();
		while (this->available() < expected_len)
		{
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();

			if (elapsed > 1000)
			{
				std::cout << "Timeout waiting for data. Expected: " << static_cast<int>(expected_len) << ", Available: " << this->available() << std::endl;
				break;
			}

			usleep(thread_timeout * 10);
		}

		bytes_read = this->receive(rx_buff + 1, expected_len);
		if (bytes_read != expected_len)
		{
			std::cout << "Failed to read complete message. Expected: " << static_cast<int>(expected_len) << ", Got: " << bytes_read << std::endl;
			continue;
		}

		size_t total_bytes = 1 + expected_len;

		try
		{
			Message mes = Message::deserialize(rx_buff, total_bytes);
			mes.print();
			std::unique_lock<std::mutex> queueLock(mtxReceive);

			notifyReceive(mes);

			queueLock.unlock();
		}
		catch (const std::exception &ex)
		{
			std::cout << "Exception: " << ex.what() << std::endl;
		}
		memset(rx_buff, 0, RX_BUFF_SIZE);
	};
};

ErrorCode UartTransport::close()
{
    if (!is_open())  
    {  
        return ErrorCode::Success;  
    }  

    if (m_fd >= 0)  
    {  
        ::close(m_fd);  
        m_fd = -1;  
    }  

	m_con_state = ConnectionState::Closed;
	return ErrorCode::Success;
}

int UartTransport::sendMessage(const Message *mes)
{
	auto serialized = mes->serialize();

	int status = this->send(serialized.data(), serialized.size());
	return status;
}

int UartTransport::send(const char *data, size_t length)
{
	if (!is_open())
	{
		throw PortException("Port not open", ErrorCode::PortNotOpen);
	}

	if (data == nullptr || length == 0)
	{
		return 0;
	}

	std::cout << "Sending: " << length << " bytes" << std::endl;

	ssize_t bytes_written = ::write(m_fd, data, length);
	if (bytes_written < 0)
	{
		throw PortException("Failed to write to port", ErrorCode::OperationFailed);
	}
	std::cout << "Written bytes: " << bytes_written << std::endl;
	return bytes_written;
}

int UartTransport::receive(char *buffer, size_t length)
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
	options.c_cc[VTIME] = 100;

	if (tcsetattr(m_fd, TCSANOW, &options) != 0)
	{
		::close(m_fd);
		m_fd = -1;
		return ErrorCode::InvalidParameter;
	}

	return ErrorCode::Success;
}
