import serial
import re

ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)

while True:
    com_input = ser.read(1)
    if com_input:
        com_input = str(com_input)
        pattern = re.compile('xba(.+)\'')
        com_input = pattern.findall(com_input)
        print(com_input)