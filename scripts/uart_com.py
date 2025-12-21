import time
import serial
from dataclasses import dataclass
from typing import List, Optional

COM_PORT = "/tmp/tty20"
BAUDRATE = 115200

serial_con = None

@dataclass
class Message:
    idx: int
    data: bytes
    
    def __str__(self):
        data_hex = " ".join([f"{b:02X}" for b in self.data])
        try:
            data_ascii = self.data.decode('ascii', errors='replace')
        except:
            data_ascii = "<binary>"
        return f"Message(idx=0x{self.idx:08X}, data=[{data_hex}] '{data_ascii}')"
    
    def serialize(self) -> bytes:
        """Serialize message to bytes matching C++ format"""
        length = 4 + len(self.data)
        
        buffer = bytearray()
        
        buffer.append(length & 0xFF)
        
        buffer.append((self.idx >> 24) & 0xFF)
        buffer.append((self.idx >> 16) & 0xFF)
        buffer.append((self.idx >> 8) & 0xFF)
        buffer.append((self.idx >> 0) & 0xFF)
        
        buffer.extend(self.data)
        
        return bytes(buffer)
    
    @staticmethod
    def deserialize(buffer: bytes) -> Optional['Message']:
        """Deserialize bytes to Message"""
        if len(buffer) < 5:
            return None
        
        length = buffer[0]
        
        idx = (buffer[1] << 24) | \
              (buffer[2] << 16) | \
              (buffer[3] << 8) | \
              (buffer[4] << 0)
        
        data = buffer[5:]
        
        return Message(idx=idx, data=data)


class MessageReceiver:
    def __init__(self):
        self.buffer = bytearray()
        self.STATE_IDLE = 0
        self.STATE_RECEIVING = 1
        self.state = self.STATE_IDLE
        self.expected_length = 0
        
    def process_byte(self, byte):
        """Process incoming byte and return Message if complete, None otherwise"""
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
        self.buffer. clear()
        self.state = self.STATE_IDLE
        self.expected_length = 0


def send_message(ser: serial.Serial, idx: int, data: bytes):
    """Send a message over serial port"""
    msg = Message(idx=idx, data=data)
    serialized = msg.serialize()
    
    print(f"Sending:  {msg}")
    print(f"Raw bytes: {' '.join([f'{b:02X}' for b in serialized])}")
    
    ser.write(serialized)
    ser.flush()


def send_string(ser: serial.Serial, idx: int, text: str):
    """Send a string message"""
    send_message(ser, idx, text.encode('ascii'))


def send_binary(ser: serial.Serial, idx: int, data: List[int]):
    """Send binary data"""
    send_message(ser, idx, bytes(data))


def connect_to_serial():
    global serial_con
    
    if serial_con is not None: 
        raise Exception("Serial port already opened")
    
    serial_con = serial.Serial(COM_PORT, BAUDRATE, timeout=0.1)
    
    serial_con.write("BUFF".encode("ASCII"))
    print("Connected and sent BUFF command\n")
    
    receiver = MessageReceiver()
    
    print("=== Sending Test Messages ===")
    send_string(serial_con, 0x12345678, "Hello")
    time.sleep(0.1)
    
    send_string(serial_con, 0xABCDEF01, "World!")
    time.sleep(0.1)
    
    send_binary(serial_con, 0x00000042, [0x01, 0x02, 0x03, 0xFF])
    time.sleep(0.1)
    
    print("\n=== Listening for Messages ===")
    
    try:
        while serial_con.is_open:
            if serial_con.in_waiting > 0:
                byte_data = serial_con. read(1)
                if len(byte_data) > 0:
                    byte_val = byte_data[0]
                    
                    message = receiver.process_byte(byte_val)
                    
                    if message is not None: 
                        print(f"Received: {message}")
            else:
                time.sleep(0.01)
                
    except KeyboardInterrupt:
        print("\n\nClosing serial connection...")
    finally:
        if serial_con is not None:
            serial_con.close()


def interactive_mode():
    """Interactive mode for sending messages"""
    global serial_con
    
    serial_con = serial.Serial(COM_PORT, BAUDRATE, timeout=0.1)
    print("Connected to serial port")
    print("Commands:")
    print("  send <idx> <text>     - Send text message")
    print("  binary <idx> <bytes>  - Send binary (e.g., binary 123 01,02,FF)")
    print("  quit                  - Exit")
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
                            print(f"\n← Received: {message}")
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
                    
                parts = user_input.split(maxsplit=2)
                
                if parts[0] == "quit":
                    break
                    
                elif parts[0] == "send" and len(parts) >= 3:
                    idx = int(parts[1], 0)
                    text = parts[2]
                    send_string(serial_con, idx, text)
                    
                elif parts[0] == "binary" and len(parts) >= 3:
                    idx = int(parts[1], 0)
                    byte_strs = parts[2]. split(',')
                    data = [int(b. strip(), 16) for b in byte_strs]
                    send_binary(serial_con, idx, data)
                    
                else:
                    print("Unknown command")
                    
            except ValueError as e:
                print(f"Error: {e}")
            except EOFError:
                break
                
    except KeyboardInterrupt: 
        print("\nExiting...")
    finally:
        if serial_con: 
            serial_con.close()


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "-i":
        interactive_mode()
    else:
        connect_to_serial()
