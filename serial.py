import serial
import time

# Set the correct COM port and baud rate q
ser = serial.Serial('COM5', 9600)

while True:
    if ser.in_waiting > 0:
        incoming = ser.readline().decode('utf-8').strip()
        if incoming == "GET_TIME":
            currentTime = time.strftime("%H:%M:%S")
            ser.write(currentTime.encode())
