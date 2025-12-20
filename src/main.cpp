#include "main.hpp"
#include "messages/Message.hpp"
#include <format>

using namespace transport;

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

	auto config = SerialConfig();
	config.baudrate = BaudRate::Baud115200;
	config.port = "/tmp/ttyS21";
	config.databits = DataBits::Bits8;
	config.stopbits = StopBits::One;
	config.parity = Parity::None;

	auto uart_transport = UartTransport(config);
	
	Message test_mes = {
		.idx = 1
	};

	test_mes.data.push_back(65);
	test_mes.data.push_back(66);
	test_mes.data.push_back(67);
	test_mes.data.push_back(68);

	auto parsed = test_mes.serialize();

	for (const auto& byte : parsed) {
		std::cout << std::format("0x{:02X} ", static_cast<uint8_t>(byte));
	}

	std::cout << std::endl;
	std::cout << "Size: " << parsed.size() << " "   << std::endl;

	auto status = uart_transport.open();
	cout << "Open status: " << static_cast<int>(status) << endl;
	if (status != ErrorCode::Success) {
		return -1;
	}


	// std::string mes = "Hello UART\n";
	// uart_transport.send(reinterpret_cast<const transport::Byte*>(mes.data()), mes.length());

	while (1)
	{
		// uart_transport.send(reinterpret_cast<const transport::Byte*>(mes.data()), mes.length());
		uart_transport.sendMessage(&test_mes);

		this_thread::sleep_for(chrono::milliseconds(2000));
	}
	return 0;
}
