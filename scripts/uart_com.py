import time
import serial
from dataclasses import dataclass

COM_PORT = "/tmp/tty20"
BAUDRATE = 115200

@dataclass
class Message:
    length: int
    idx: int
    data: bytes

def read_message(ser):
    """Read one complete message from serial port"""
    try:
        length_byte = ser. read(1)
        if len(length_byte) == 0:
            return None
            
        length = length_byte[0]
        
        remaining = ser.read(length)
        if len(remaining) < length:
            print(f"Warning: Expected {length} bytes, got {len(remaining)}")
            return None
        
        idx = (remaining[0] << 24) | \
              (remaining[1] << 16) | \
              (remaining[2] << 8) | \
              (remaining[3] << 0)
        
        data = remaining[4:]
        
        return Message(length=length, idx=idx, data=data)
        
    except Exception as e: 
        print(f"Error reading message: {e}")
        return None

def main():
    with serial.Serial(COM_PORT, BAUDRATE, timeout=1.0) as ser:
        ser.write(b"BUFF")
        print("Connected to serial port")
        
        try:
            while True:
                msg = read_message(ser)
                if msg is not None:
                    print(f"Length: {msg.length}")
                    print(f"IDX: 0x{msg. idx:08X} ({msg.idx})")
                    print(f"Data (hex): {' '.join([f'{b:02X}' for b in msg. data])}")
                    try:
                        print(f"Data (ASCII): {msg.data.decode('ascii')}")
                    except: 
                        print(f"Data (ASCII): <binary data>")
                    print("-" * 50)
                else:
                    time.sleep(0.01)
                    
        except KeyboardInterrupt:
            print("\nExiting...")

if __name__ == "__main__":
    main()