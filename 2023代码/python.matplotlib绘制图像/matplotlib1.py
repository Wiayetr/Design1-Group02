import matplotlib.pyplot as plt
import serial
import re


ax = []
ay = []
plt.ion()
ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)
x = 0
plt.figure(figsize=(15, 6))
while True:
    com_input = ser.read(19)
    if com_input:
        com_input = str(com_input)
        pattern = re.compile('xba(.+)\'')
        com_input = pattern.findall(com_input)
        print(com_input)
        if com_input:
            com_input = com_input.decode('utf-8')
            com_input = float(com_input)
            print(com_input, end='')
            ax.append(x)
            x = x + 0.2
            ay.append(com_input)
        plt.clf()
        plt.plot(ax, ay)
        plt.pause(0.001)
        plt.ioff()