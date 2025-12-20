#include "main.hpp"


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
	
	auto status = uart_transport.open();
	cout << "Open status: " << static_cast<int>(status) << endl;
	if (status != ErrorCode::Success) {
		return -1;
	}

	std::string mes = "Hello UART\n";
	uart_transport.send(reinterpret_cast<const transport::Byte*>(mes.data()), mes.length());

	while (1)
	{
		// uart_transport.send(reinterpret_cast<const transport::Byte*>(mes.data()), mes.length());

		this_thread::sleep_for(chrono::milliseconds(2000));
	}
	return 0;
}
