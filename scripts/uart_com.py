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
    data = []
    while serial_con.is_open: 
        while serial_con.inWaiting():
            ddmes = serial_con.read(1)
            if ddmes.hex() == '01':
                data = []
                
            if ddmes.hex() == '17':
                break
                            
            if ddmes.hex() == "06":
                print('ack')
                
            data.append(ddmes)
            time.sleep(0.001)
            
        if len(data) > 0:
            print(", ".join([f"0x{dat.hex()}" for dat in data]))
            print("".join([dat.decode("ASCII") for dat in data]))
            data = []
            
            
        time.sleep(0.1)

            
        # counter+=1 
        if counter % 20 == 1:
            ran = str(random.randint(1, 69) * random.randint(1,4)).encode("ASCII")
            print(f"Sending {ran}")
            serial_con.write(ran)
            
if __name__ == "__main__":
    serial_con = None
    
    connect_to_serial()