#include "main.hpp"


using namespace wm::transport;
using namespace wm::protoc;
using namespace wm::messages;
using namespace wm::devices;
using namespace std;

void runTestDevice(ITransport *transport, IProtocolAdapter *protocol)
{
	cout << "=== TestDevice Protocol Demo ===" << endl;

	TestDevice test_device(transport, protocol);
	test_device.connect();
	cout << "\n=== Sending Test Messages ===" << endl;

	{
		vector<char> cmd_data = {'H', 'E', 'L', 'L', 'O'};
		cout << "\nSending COMMAND message" << endl;
		test_device.sendCommand(0x00000001, cmd_data);
	}

	{
		vector<char> data = {0x01, 0x02, 0x03, static_cast<char>(0xFF)};
		cout << "\nSending DATA message with binary payload" << endl;
		test_device.sendData(0x00000002, data);
	}

	{
		vector<char> resp = {'O', 'K'};
		cout << "\nSending RESPONSE message" << endl;
		test_device.sendResponse(0x00000003, resp);
	}

	{
		cout << "\nSending HEARTBEAT message" << endl;
		test_device.sendHeartbeat(0x00000004);
	}

	{
		vector<char> large_data;
		large_data.reserve(100);
		for (int i = 0; i < 100; ++i)
		{
			large_data.push_back(static_cast<char>(i % 256));
		}
		cout << "\nSending large DATA message (100 bytes)" << endl;
		test_device.sendData(0x00000005, large_data);
	}

	cout << "\n=== All tests completed ===" << endl;

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	test_device.disconnect();

}

void runLedController(ITransport *transport, IProtocolAdapter *protocol)
{
	cout << "=== LedController Protocol Demo ===" << endl;

	LedControllerDevice led_device(transport, protocol);
	led_device.connect();
	cout << "\n=== LED Control Tests ===" << endl;

	{
		cout << "\nTurn LED ON" << endl;
		led_device.turnOn();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	{
		cout << "\nTurn LED OFF" << endl;
		led_device.turnOff();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	{
		cout << "\nSet brightness to 50%" << endl;
		led_device.setBrightness(128);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	{
		cout << "\nSet brightness to 100%" << endl;
		led_device.setBrightness(255);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	{
		cout << "\nBlink pattern - ON/OFF cycle" << endl;
		for (int i = 0; i < 3; ++i)
		{
			cout << "  Blink " << (i + 1) << "/3" << endl;
			led_device.turnOn();
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			led_device.turnOff();
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	}

	{
		cout << "\nBrightness fade sequence" << endl;
		for (int level = 0; level <= 255; level += 51)
		{
			cout << "  Setting brightness to: " << level << endl;
			led_device.setBrightness(level);
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}

	cout << "\n=== All tests completed ===" << endl;
	led_device.disconnect();
}

int main(int argc, char *argv[])
{
	auto config = SerialConfig();
	config.baudrate = BaudRate::Baud115200;
	config.port = "/tmp/ttyS21";
	config.databits = DataBits::Bits8;
	config.stopbits = StopBits::One;
	config.parity = Parity::None;

	auto uart_transport = UartTransport(config);

	IProtocolAdapter *protocol = nullptr;
	string device_choice = "test";
	string protocol_choice = "plain";
	uint16_t shift_value = 0x69;

	if (argc == 1)
	{
		protocol_choice = "plain";
		device_choice = "test";
	}
	else if (argc == 2)
	{
		string arg1 = argv[1];
		if (arg1 == "led")
		{
			protocol_choice = "plain";
			device_choice = "led";
		}
		else if (arg1 == "plain")
		{
			protocol_choice = "plain";
			device_choice = "test";
		}
		else if (arg1 == "shift")
		{
			protocol_choice = "shift";
			device_choice = "test";
		}
		else
		{
			protocol_choice = "plain";
			device_choice = "test";
		}
	}
	else if (argc == 3)
	{
		string arg1 = argv[1];
		string arg2 = argv[2];

		if (arg1 == "plain" || arg1 == "shift")
		{
			protocol_choice = arg1;
			device_choice = arg2;
		}
		else if (arg1 == "shift")
		{
			protocol_choice = "shift";
			shift_value = static_cast<uint16_t>(std::stoi(arg2, nullptr, 0));
			device_choice = "test";
		}
		else
		{
			protocol_choice = arg1;
			device_choice = arg2;
		}
	}
	else if (argc >= 4)
	{
		string arg1 = argv[1];
		string arg2 = argv[2];
		string arg3 = argv[3];

		protocol_choice = arg1;
		shift_value = static_cast<uint16_t>(std::stoi(arg2, nullptr, 0));
		device_choice = arg3;
	}

	if (protocol_choice == "shift")
	{
		std::cout << "Using ShiftProtocol with shift value: 0x" << std::hex << shift_value << std::dec << std::endl;
		protocol = new ShiftProtocol(shift_value);
	}
	else
	{
		std::cout << "Using PlainProtocol" << std::endl;
		protocol = new PlainProtocol();
	}

	if (device_choice == "led")
	{
		runLedController(&uart_transport, protocol);
	}
	else
	{
		runTestDevice(&uart_transport, protocol);
	}

	delete protocol;
	return 0;
}
