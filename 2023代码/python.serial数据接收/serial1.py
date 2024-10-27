import serial
import re

ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)

while True:
    com_input = ser.read(42)
    if com_input:
        com_input = com_input.str('utf-8')
        print(com_input)
