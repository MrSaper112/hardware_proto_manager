from enum import IntEnum
import time
import serial
from dataclasses import dataclass
from typing import List, Optional
from abc import ABC, abstractmethod

COM_PORT = "/tmp/tty20"
BAUDRATE = 115200

serial_con = None


class MessageType(IntEnum):
    """Message type enumeration matching C++ MessageType"""
    NONE = 0x00
    COMMAND = 0x01
    RESPONSE = 0x02
    DATA = 0x03
    HEARTBEAT = 0x04
    ERROR = 0x05
    Undefined = 0xFF


def message_type_to_string(msg_type: MessageType) -> str:
    """Convert MessageType to human-readable string"""
    mapping = {
        MessageType.NONE: "NONE",
        MessageType.COMMAND: "COMMAND",
        MessageType.RESPONSE: "RESPONSE",
        MessageType.DATA: "DATA",
        MessageType.HEARTBEAT: "HEARTBEAT",
        MessageType.ERROR: "ERROR",
        MessageType.Undefined: "UNDEFINED",
    }
    return mapping.get(msg_type, "UNKNOWN")


@dataclass
class Message:
    """Message class matching C++ Message structure"""
    idx: int
    mesType: MessageType
    data: bytes
    
    def __str__(self):
        data_hex = " ".join([f"{b:02X}" for b in self.data])
        try:
            data_ascii = self.data.decode('ascii', errors='replace')
        except:
            data_ascii = "<binary>"
        return f"Message(idx=0x{self.idx:08X}, type=[{message_type_to_string(self.mesType)}] data=[{data_hex}] '{data_ascii}')"
    
    def serialize(self) -> bytes:
        """Serialize message to bytes matching C++ format
        
        Format:
        - Length (1 byte): 5 + len(data)
        - Type (1 byte): MessageType
        - Index (4 bytes): Big-endian uint32
        - Data (variable): Payload
        """
        length = 5 + len(self.data)
        
        buffer = bytearray()
        buffer.append(length & 0xFF)
        buffer.append(self.mesType & 0xFF)
        buffer.append((self.idx >> 24) & 0xFF)
        buffer.append((self.idx >> 16) & 0xFF)
        buffer.append((self.idx >> 8) & 0xFF)
        buffer.append((self.idx >> 0) & 0xFF)
        buffer.extend(self.data)
        
        return bytes(buffer)
    
    @staticmethod
    def deserialize(buffer: bytes) -> Optional['Message']:
        """Deserialize bytes to Message"""
        if len(buffer) < 6:
            return None
        
        length = buffer[0]
        mesType = MessageType(buffer[1])
        
        idx = (buffer[2] << 24) | \
              (buffer[3] << 16) | \
              (buffer[4] << 8) | \
              (buffer[5] << 0)
        
        data = buffer[6:]
        
        return Message(idx=idx, mesType=mesType, data=data)


# =====================
# Protocol Adapters
# =====================

class IProtocolAdapter(ABC):
    """Base protocol adapter interface"""
    
    @abstractmethod
    def encode(self, message: Message) -> bytes:
        """Encode a message using this protocol"""
        pass
    
    @abstractmethod
    def decode(self, data: bytes) -> Message:
        """Decode a message using this protocol"""
        pass
    
    def create_command(self, idx: int, data: bytes) -> Message:
        """Create a command message"""
        return Message(idx=idx, mesType=MessageType.COMMAND, data=data)
    
    def create_response(self, idx: int, data: bytes) -> Message:
        """Create a response message"""
        return Message(idx=idx, mesType=MessageType.RESPONSE, data=data)
    
    def create_data_message(self, idx: int, data: bytes) -> Message:
        """Create a data message"""
        return Message(idx=idx, mesType=MessageType.DATA, data=data)
    
    def create_error(self, idx: int, data: bytes) -> Message:
        """Create an error message"""
        return Message(idx=idx, mesType=MessageType.ERROR, data=data)
    
    def create_heartbeat(self, idx: int) -> Message:
        """Create a heartbeat message"""
        return Message(idx=idx, mesType=MessageType.HEARTBEAT, data=b'')


class PlainProtocol(IProtocolAdapter):
    """Plain protocol: No encoding, just serialize the message directly"""
    
    def __init__(self):
        pass
    
    def encode(self, message: Message) -> bytes:
        """Encode message by serializing it directly"""
        return message.serialize()
    
    def decode(self, data: bytes) -> Message:
        """Decode message by deserializing it directly"""
        msg = Message.deserialize(data)
        if msg is None:
            raise ValueError("Failed to deserialize message")
        return msg
    
    def __repr__(self):
        return "PlainProtocol()"


class ShiftProtocol(IProtocolAdapter):
    """Shift protocol: Applies XOR-like shift to data payload before serialization"""
    
    def __init__(self, char_shift: int = 0x00):
        """Initialize shift protocol with a character shift value
        
        Args:
            char_shift: Value to shift each data byte by (0-255)
        """
        self.char_shift = char_shift & 0xFF
    
    def _encode_byte(self, byte: int) -> int:
        """Encode a single byte"""
        return (byte + self.char_shift) % 256
    
    def _decode_byte(self, byte: int) -> int:
        """Decode a single byte"""
        return (byte - self.char_shift + 256) % 256
    
    def encode(self, message: Message) -> bytes:
        """Encode message by shifting data payload, then serializing"""
        # Shift all data bytes
        shifted_data = bytes([self._encode_byte(b) for b in message.data])
        
        # Create new message with shifted data
        shifted_message = Message(
            idx=message.idx,
            mesType=message.mesType,
            data=shifted_data
        )
        
        # Serialize the shifted message
        return shifted_message.serialize()
    
    def decode(self, data: bytes) -> Message:
        """Decode message by deserializing, then unshifting data payload"""
        # First deserialize
        msg = Message.deserialize(data)
        if msg is None:
            raise ValueError("Failed to deserialize message")
        
        # Then unshift the data bytes
        unshifted_data = bytes([self._decode_byte(b) for b in msg.data])
        
        # Create new message with unshifted data
        return Message(
            idx=msg.idx,
            mesType=msg.mesType,
            data=unshifted_data
        )
    
    def __repr__(self):
        return f"ShiftProtocol(shift=0x{self.char_shift:02X})"


class MessageReceiver:
    """Receives and reassembles messages from serial stream"""
    
    STATE_IDLE = 0
    STATE_RECEIVING = 1
    
    def __init__(self):
        self.buffer = bytearray()
        self.state = self.STATE_IDLE
        self.expected_length = 0
        
    def process_byte(self, byte: int) -> Optional[Message]:
        """Process incoming byte and return Message if complete, None otherwise
        
        Args:
            byte: Single byte value (0-255)
            
        Returns:
            Message if complete message received, None otherwise
        """
        self.buffer.append(byte)
        
        if self.state == self.STATE_IDLE: 
            self.expected_length = byte
            self.state = self.STATE_RECEIVING
            return None
            
        elif self.state == self.STATE_RECEIVING:
            if len(self.buffer) >= (1 + self.expected_length):
                # Complete message received
                msg = Message.deserialize(bytes(self.buffer))
                self.reset()
                return msg
                
        return None
    
    def reset(self):
        """Reset receiver state"""
        self.buffer.clear()
        self.state = self.STATE_IDLE
        self.expected_length = 0


def send(ser: serial.Serial, idx: int, message_type: MessageType, data: bytes, 
         protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send a message over serial port
    
    Args:
        ser: Serial port connection
        idx: Message index (uint32)
        message_type: Type of message (MessageType enum)
        data: Payload data as bytes
        protocol: Protocol adapter to use (defaults to PlainProtocol)
        verbose: Print debug information
    """
    if protocol is None:
        protocol = PlainProtocol()
    
    msg = Message(idx=idx, mesType=message_type, data=data)
    encoded = protocol.encode(msg)
    
    if verbose:
        print(f"\n=== Sending Message ===")
        print(f"Protocol: {protocol}")
        print(f"Message: {msg}")
        print(f"Raw bytes: {' '.join([f'{b:02X}' for b in encoded])}")
    
    ser.write(encoded)
    ser.flush()


def send_string(ser: serial.Serial, idx: int, message_type: MessageType, text: str,
                protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send a string message
    
    Args:
        ser: Serial port connection
        idx: Message index
        message_type: Type of message
        text: String to send
        protocol: Protocol adapter
        verbose: Print debug information
    """
    send(ser, idx, message_type, text.encode('ascii'), protocol, verbose)


def send_binary(ser: serial.Serial, idx: int, message_type: MessageType, 
                data: List[int], protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send binary data
    
    Args:
        ser: Serial port connection
        idx: Message index
        message_type: Type of message
        data: List of byte values (0-255)
        protocol: Protocol adapter
        verbose: Print debug information
    """
    send(ser, idx, message_type, bytes(data), protocol, verbose)


def send_command(ser: serial.Serial, idx: int, data: bytes, 
                 protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send a COMMAND type message"""
    send(ser, idx, MessageType.COMMAND, data, protocol, verbose)


def send_response(ser: serial.Serial, idx: int, data: bytes,
                  protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send a RESPONSE type message"""
    send(ser, idx, MessageType.RESPONSE, data, protocol, verbose)


def send_data(ser: serial.Serial, idx: int, data: bytes,
              protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send a DATA type message"""
    send(ser, idx, MessageType.DATA, data, protocol, verbose)


def send_error(ser: serial.Serial, idx: int, data: bytes,
               protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send an ERROR type message"""
    send(ser, idx, MessageType.ERROR, data, protocol, verbose)


def send_heartbeat(ser: serial.Serial, idx: int,
                   protocol: IProtocolAdapter = None, verbose: bool = True):
    """Send a HEARTBEAT type message"""
    send(ser, idx, MessageType.HEARTBEAT, b'', protocol, verbose)



def connect_to_serial(port: str = COM_PORT, baudrate: int = BAUDRATE, 
                     protocol: IProtocolAdapter = None):
    """Connect to serial port and run test message exchange
    
    Args:
        port: Serial port path
        baudrate: Baud rate
        protocol: Protocol adapter to use
    """
    global serial_con
    
    if serial_con is not None: 
        raise Exception("Serial port already opened")
    
    if protocol is None:
        protocol = PlainProtocol()
    
    serial_con = serial.Serial(port, baudrate, timeout=0.1)
    print(f"Connected to {port} at {baudrate} baud")
    print(f"Using protocol: {protocol}\n")
    
    receiver = MessageReceiver()
    
    print("=== Sending Test Messages ===")
    send_string(serial_con, 0x12345678, MessageType.COMMAND, "Hello", protocol)
    time.sleep(0.1)
    
    send_string(serial_con, 0xABCDEF01, MessageType.COMMAND, "Maja!", protocol)
    time.sleep(0.1)
    
    import threading
        
    def receive_loop():
        while serial_con and serial_con.is_open:
            if serial_con.in_waiting > 0:
                byte_data = serial_con.read(1)
                if len(byte_data) > 0:
                    message = receiver.process_byte(byte_data[0])
                    if message is not None:
                        try:
                            decoded = protocol.decode(message.serialize())
                            print(f"\n← Received: {decoded}")
                        except:
                            print(f"\n← Received (raw): {message}")
            else:
                time.sleep(0.01)
        
    recv_thread = threading.Thread(target=receive_loop, daemon=True)
    recv_thread.start()

    print("\n=== Sending periodic data messages ===")
    idx = 0
    try:
        while serial_con.is_open:
            send_heartbeat(serial_con, idx, protocol, verbose=True)
            time.sleep(5)
            idx += 1
    except KeyboardInterrupt:
        print("\n\nClosing serial connection...")
    finally:
        if serial_con is not None:
            serial_con.close()


def interactive_mode(port: str = COM_PORT, baudrate: int = BAUDRATE,
                     protocol: IProtocolAdapter = None):
    """Interactive mode for sending messages
    
    Commands:
        send <idx> <type> <text>    - Send text message (e.g., send 1 1 ABCD)
        cmd <idx> <text>            - Send COMMAND (e.g., cmd 1 ABCD)
        resp <idx> <text>           - Send RESPONSE
        data <idx> <text>           - Send DATA
        error <idx> <text>          - Send ERROR
        hb <idx>                    - Send HEARTBEAT
        binary <idx> <type> <hex>   - Send binary (e.g., binary 123 1 01,02,FF)
        quit                        - Exit
    """
    global serial_con
    
    if protocol is None:
        protocol = PlainProtocol()
    
    serial_con = serial.Serial(port, baudrate, timeout=0.1)
    print(f"Connected to {port} at {baudrate} baud")
    print(f"Using protocol: {protocol}")
    print("\nAvailable commands:")
    print("  send <idx> <type> <text>      - Send text message")
    print("  cmd <idx> <text>              - Send COMMAND message")
    print("  resp <idx> <text>             - Send RESPONSE message")
    print("  data <idx> <text>             - Send DATA message")
    print("  error <idx> <text>            - Send ERROR message")
    print("  hb <idx>                      - Send HEARTBEAT message")
    print("  binary <idx> <type> <hex>    - Send binary (e.g., binary 1 1 01,02,FF)")
    print("  quit                          - Exit")
    print()
    
    receiver = MessageReceiver()
    
    try:
        import threading
        
        def receive_loop():
            while serial_con and serial_con.is_open:
                if serial_con.in_waiting > 0:
                    byte_data = serial_con.read(1)
                    if len(byte_data) > 0:
                        message = receiver.process_byte(byte_data[0])
                        if message is not None:
                            try:
                                decoded = protocol.decode(message.serialize())
                                print(f"\n← Received: {decoded}")
                            except:
                                print(f"\n← Received (raw): {message}")
                            print("> ", end='', flush=True)
                else:
                    time.sleep(0.01)
        
        recv_thread = threading.Thread(target=receive_loop, daemon=True)
        recv_thread.start()
        
        while True:
            try:
                user_input = input("> ").strip()
                
                if not user_input:
                    continue
                    
                parts = user_input.split(maxsplit=3)
                
                if parts[0] == "quit":
                    break
                    
                elif parts[0] == "send" and len(parts) >= 4:
                    idx = int(parts[1], 0)
                    msg_type = MessageType(int(parts[2], 0))
                    text = parts[3]
                    send_string(serial_con, idx, msg_type, text, protocol)
                    
                elif parts[0] == "cmd" and len(parts) >= 3:
                    idx = int(parts[1], 0)
                    text = " ".join(parts[2:])
                    send_command(serial_con, idx, text.encode('ascii'), protocol)
                    
                elif parts[0] == "resp" and len(parts) >= 3:
                    idx = int(parts[1], 0)
                    text = " ".join(parts[2:])
                    send_response(serial_con, idx, text.encode('ascii'), protocol)
                    
                elif parts[0] == "data" and len(parts) >= 3:
                    idx = int(parts[1], 0)
                    text = " ".join(parts[2:])
                    send_data(serial_con, idx, text.encode('ascii'), protocol)
                    
                elif parts[0] == "error" and len(parts) >= 3:
                    idx = int(parts[1], 0)
                    text = " ".join(parts[2:])
                    send_error(serial_con, idx, text.encode('ascii'), protocol)
                    
                elif parts[0] == "hb" and len(parts) >= 2:
                    idx = int(parts[1], 0)
                    send_heartbeat(serial_con, idx, protocol)
                    
                elif parts[0] == "binary" and len(parts) >= 4:
                    idx = int(parts[1], 0)
                    msg_type = MessageType(int(parts[2], 0))
                    byte_strs = parts[3].split(',')
                    data = [int(b.strip(), 16) for b in byte_strs]
                    send_binary(serial_con, idx, msg_type, data, protocol)
                    
                else:
                    print("Unknown command or invalid arguments")
                    
            except ValueError as e:
                print(f"Error: Invalid value - {e}")
            except Exception as e:
                print(f"Error: {e}")
            except EOFError:
                break
                
    except KeyboardInterrupt: 
        print("\nExiting...")
    finally:
        if serial_con: 
            serial_con.close()
            print("Serial port closed")



def read_mode(port: str = COM_PORT, baudrate: int = BAUDRATE, 
              protocol: IProtocolAdapter = None, timeout: float = None):
    """Read-only mode: Listen for messages
    
    Args:
        port: Serial port path
        baudrate: Baud rate
        protocol: Protocol adapter
        timeout: Timeout in seconds (None = infinite)
    """
    global serial_con
    
    if protocol is None:
        protocol = PlainProtocol()
    
    serial_con = serial.Serial(port, baudrate, timeout=0.1)
    print(f"Connected to {port} at {baudrate} baud (READ-ONLY)")
    print(f"Using protocol: {protocol}")
    print("Listening for messages (press Ctrl+C to exit)...\n")
    
    receiver = MessageReceiver()
    start_time = time.time() if timeout else None
    message_count = 0
    
    try:
        while serial_con.is_open:
            if timeout and (time.time() - start_time) > timeout:
                print(f"\nTimeout reached ({timeout}s)")
                break
            
            if serial_con.in_waiting > 0:
                byte_data = serial_con.read(1)
                if len(byte_data) > 0:
                    message = receiver.process_byte(byte_data[0])
                    if message is not None:
                        message_count += 1
                        try:
                            decoded = protocol.decode(message.serialize())
                            print(f"[{message_count}] ← Received: {decoded}")
                        except:
                            print(f"[{message_count}] ← Received (raw): {message}")
            else:
                time.sleep(0.01)
    
    except KeyboardInterrupt:
        print(f"\n\nReceived {message_count} messages total")
    finally:
        if serial_con:
            serial_con.close()
            print("Serial port closed")


def send_mode(port: str = COM_PORT, baudrate: int = BAUDRATE,
              protocol: IProtocolAdapter = None, count: int = 1, interval: float = 1.0):
    """Send-only mode: Send test messages
    
    Args:
        port: Serial port path
        baudrate: Baud rate
        protocol: Protocol adapter
        count: Number of messages to send
        interval: Delay between messages in seconds
    """
    global serial_con
    
    if protocol is None:
        protocol = PlainProtocol()
    
    serial_con = serial.Serial(port, baudrate, timeout=0.1)
    print(f"Connected to {port} at {baudrate} baud (SEND-ONLY)")
    print(f"Using protocol: {protocol}")
    print(f"Sending {count} messages with {interval}s interval\n")
    
    try:
        for i in range(count):
            idx = 0x10000000 + i
            data = f"Message {i+1}".encode('ascii')
            send_data(serial_con, idx=idx, data=data, protocol=protocol, verbose=True)
            
            if i < count - 1:
                time.sleep(interval)
        
        print(f"\nSent {count} messages successfully")
    
    except KeyboardInterrupt:
        print("\n\nSend interrupted")
    finally:
        if serial_con:
            serial_con.close()
            print("Serial port closed")


def send_recv_mode(port: str = COM_PORT, baudrate: int = BAUDRATE,
                   protocol: IProtocolAdapter = None):
    """Send and receive mode: Send a message and wait for response
    
    Args:
        port: Serial port path
        baudrate: Baud rate
        protocol: Protocol adapter
    """
    global serial_con
    
    if protocol is None:
        protocol = PlainProtocol()
    
    serial_con = serial.Serial(port, baudrate, timeout=0.1)
    print(f"Connected to {port} at {baudrate} baud (SEND & RECEIVE)")
    print(f"Using protocol: {protocol}")
    print("Send a message, wait for response (press Ctrl+C to exit)\n")
    
    receiver = MessageReceiver()
    message_count = 0
    
    try:
        import threading
        
        def receive_loop():
            nonlocal message_count
            while serial_con and serial_con.is_open:
                if serial_con.in_waiting > 0:
                    byte_data = serial_con.read(1)
                    if len(byte_data) > 0:
                        message = receiver.process_byte(byte_data[0])
                        if message is not None:
                            message_count += 1
                            try:
                                decoded = protocol.decode(message.serialize())
                                print(f"\n← Received: {decoded}")
                            except:
                                print(f"\n← Received (raw): {message}")
                            print("> ", end='', flush=True)
                else:
                    time.sleep(0.01)
        
        recv_thread = threading.Thread(target=receive_loop, daemon=True)
        recv_thread.start()
        
        while True:
            try:
                user_input = input("> ").strip()
                
                if not user_input:
                    continue
                
                parts = user_input.split(maxsplit=3)
                
                if parts[0] == "quit" or parts[0] == "exit":
                    break
                elif parts[0] == "help":
                    print("Commands: send <text>, binary <hex>, quit")
                elif parts[0] == "send" and len(parts) >= 2:
                    text = " ".join(parts[1:])
                    send_data(serial_con, idx=0x00000001, 
                             data=text.encode('ascii'), protocol=protocol)
                elif parts[0] == "binary" and len(parts) >= 2:
                    byte_strs = parts[1].split(',')
                    data = [int(b.strip(), 16) for b in byte_strs]
                    send_data(serial_con, idx=0x00000001, data=bytes(data), protocol=protocol)
                else:
                    print("Unknown command. Type 'help' for options")
            
            except ValueError as e:
                print(f"Error: {e}")
            except EOFError:
                break
        
        print(f"\nReceived {message_count} messages total")
    
    except KeyboardInterrupt:
        print(f"\n\nReceived {message_count} messages total")
    finally:
        if serial_con:
            serial_con.close()
            print("Serial port closed")


if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(
        description='UART Serial Communication Tool',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python uart_com.py --mode interactive              Interactive mode
  python uart_com.py --mode interactive --protocol shift --shift 0x42
  python uart_com.py --mode read                     Read-only mode
  python uart_com.py --mode read --timeout 10        Read with 10s timeout
  python uart_com.py --mode send --count 5 --interval 0.5
  python uart_com.py --mode send-recv                Send and receive
  python uart_com.py --port /dev/ttyUSB0 --baudrate 9600 --mode interactive
        """
    )
    
    parser.add_argument(
        '--port', '-p',
        default=COM_PORT,
        help=f'Serial port path (default: {COM_PORT})'
    )
    
    parser.add_argument(
        '--baudrate', '-b',
        type=int,
        default=BAUDRATE,
        help=f'Baud rate (default: {BAUDRATE})'
    )
    
    parser.add_argument(
        '--mode', '-m',
        choices=['interactive', 'read', 'send', 'send-recv', 'test'],
        default='test',
        help='Operation mode (default: test)'
    )
    
    parser.add_argument(
        '--protocol',
        choices=['plain', 'shift'],
        default='plain',
        help='Protocol adapter to use (default: plain)'
    )
    
    parser.add_argument(
        '--shift',
        type=lambda x: int(x, 0),
        default=0x42,
        help='Shift value for ShiftProtocol in hex or decimal (default: 0x42)'
    )
    
    parser.add_argument(
        '--count', '-c',
        type=int,
        default=5,
        help='Number of messages to send in send mode (default: 5)'
    )
    
    parser.add_argument(
        '--interval', '-i',
        type=float,
        default=1.0,
        help='Interval between messages in seconds (default: 1.0)'
    )
    
    parser.add_argument(
        '--timeout', '-t',
        type=float,
        default=None,
        help='Timeout for read mode in seconds (default: no timeout)'
    )
    
    args = parser.parse_args()
    
    # Create protocol instance
    if args.protocol == 'shift':
        protocol = ShiftProtocol(char_shift=args.shift)
    else:
        protocol = PlainProtocol()
    
    print(f"Protocol: {protocol}")
    print(f"Port: {args.port} @ {args.baudrate} baud\n")
    
    # Execute requested mode
    if args.mode == 'interactive':
        interactive_mode(port=args.port, baudrate=args.baudrate, protocol=protocol)
    elif args.mode == 'read':
        read_mode(port=args.port, baudrate=args.baudrate, protocol=protocol, timeout=args.timeout)
    elif args.mode == 'send':
        send_mode(port=args.port, baudrate=args.baudrate, protocol=protocol, 
                 count=args.count, interval=args.interval)
    elif args.mode == 'send-recv':
        send_recv_mode(port=args.port, baudrate=args.baudrate, protocol=protocol)
    elif args.mode == 'test':
        connect_to_serial(port=args.port, baudrate=args.baudrate, protocol=protocol)
