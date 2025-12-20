import time
import serial


COM_PORT = "/dev/ttyS20"
BAUDRATE = 9200

serial_con = None

def connect_to_serial():
    global serial_con
    
    if serial_con is not None: 
        raise Exception("Serial port opened")
    
    serial_con = serial.Serial(COM_PORT, BAUDRATE)
    
    serial_con.write("BUFF".encode("UTF-8"))

    while serial_con.is_open: 
        if serial_con.inWaiting():
            buff = serial_con.read(1)
                
            print(buff.decode("ASCII"))
            time.sleep(0.1)

if __name__ == "__main__":
    serial_con = None
    
    connect_to_serial()