import serial
import re

ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)

while True:
    com_input = ser.read(1)
    if com_input:
        com_input = com_input.decode('utf-8')
        if com_input == '.':
            a = ser.read(1)
            b = ser.read(1)
            c = ser.read(1)
            com_input = ser.read(5)
            if com_input:
                com_input = com_input.decode('utf-8')
                com_input = float(com_input)
                print(com_input)