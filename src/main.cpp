#include "main.hpp"
#include "transport/ITransport.hpp"
#include "protocols/IProtocolAdapter.hpp"
#include "devices/IDevice.hpp"

using namespace std;

int main()
{
	cout << "CMake Project Template" << endl;

#ifdef __unix__
	cout << "Running on Unix-like OS." << endl;
#elif defined(_WIN32) || defined(_WIN64)
	cout << "Running on Windows OS." << endl;
#else
	cout << "Unknown OS." << endl;
#endif

	cout << "Hello CMake." << endl;
	return 0;
}
