#include "main.hpp"


using namespace transport;

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

	auto config = SerialConfig();
	config.baudrate = BaudRate::Baud115200;
	config.port = "/dev/ttyUSB0";
	config.databits = DataBits::Bits8;
	config.stopbits = StopBits::One;
	config.parity = Parity::None;

	auto uart_transport = UartTransport(config);
	
	auto status = uart_transport.open();
	cout << "Open status: " << static_cast<int>(status) << endl;

	return 0;
}
