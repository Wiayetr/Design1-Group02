import serial
import re

# 串口配置
ser = serial.Serial(
    port="COM5", # 串口 COM 号设置为 5 
    baudrate=9600, # msp430 设置的波特率
    stopbits=1, 
    timeout=1
)

while True:
    com_input = ser.read(42)
    if com_input:
        com_input = com_input.decode('utf-8')
        print(com_input)
