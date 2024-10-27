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
                ax.append(x)
                x = x + 0.2
                ay.append(com_input)
            plt.clf()
            plt.plot(ax, ay)
            plt.pause(0.001)
            plt.ioff()

