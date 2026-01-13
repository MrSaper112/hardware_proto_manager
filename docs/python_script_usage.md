# Python Helper script


## Installation 

First create .venv:
```bash
cd scripts
python3 -m venv .venv
source .venv/bin/activate
pip -r req.txt
```

## Usage: 

Usage: 
```bash
uart_com.py [-h] [--port PORT] [--baudrate BAUDRATE] [--mode {interactive,read,send,send-recv,test}] [--protocol {plain,shift}]
                   [--shift SHIFT] [--count COUNT] [--interval INTERVAL] [--timeout TIMEOUT]

UART Serial Communication Tool

options:
  -h, --help            show this help message and exit
  --port PORT, -p PORT  Serial port path (default: /tmp/tty20)
  --baudrate BAUDRATE, -b BAUDRATE
                        Baud rate (default: 115200)
  --mode {interactive,read,send,send-recv,test}, -m {interactive,read,send,send-recv,test}
                        Operation mode (default: test)
  --protocol {plain,shift}
                        Protocol adapter to use (default: plain)
  --shift SHIFT         Shift value for ShiftProtocol in hex or decimal (default: 0x42)
  --count COUNT, -c COUNT
                        Number of messages to send in send mode (default: 5)
  --interval INTERVAL, -i INTERVAL
                        Interval between messages in seconds (default: 1.0)
  --timeout TIMEOUT, -t TIMEOUT
                        Timeout for read mode in seconds (default: no timeout)

Examples:
  python uart_com.py --mode interactive              Interactive mode
  python uart_com.py --mode interactive --protocol shift --shift 0x42
  python uart_com.py --mode read                     Read-only mode
  python uart_com.py --mode read --timeout 10        Read with 10s timeout
  python uart_com.py --mode send --count 5 --interval 0.5
  python uart_com.py --mode send-recv                Send and receive
  python uart_com.py --port /dev/ttyUSB0 --baudrate 9600 --mode interactive
```