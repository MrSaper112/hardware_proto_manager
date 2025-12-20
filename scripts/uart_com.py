import time
import serial
import random

COM_PORT = "/tmp/tty20"
BAUDRATE = 115200

serial_con = None

def connect_to_serial():
    global serial_con
    
    if serial_con is not None: 
        raise Exception("Serial port opened")
    
    serial_con = serial.Serial(COM_PORT, BAUDRATE)
    
    serial_con.write("BUFF".encode("ASCII"))

    counter = 0
    mes = ""
    while serial_con.is_open: 
        while serial_con.inWaiting():
            mes += serial_con.read(1).decode("ASCII")
                        
            time.sleep(0.001)
        time.sleep(0.1)
        if len(mes) > 0: 
            print(f"Recived {mes}")
            mes = ""
            
        counter+=1 
        if counter % 20 == 0:
            ran = str(random.randint(1, 69) * random.randint(1,4)).encode("ASCII")
            print(f"Sending {ran}")
            serial_con.write(ran)
            
if __name__ == "__main__":
    serial_con = None
    
    connect_to_serial()