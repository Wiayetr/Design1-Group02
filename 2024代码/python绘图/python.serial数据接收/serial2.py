'''
测试是否可以使用正则表达式对数据进行提取
'''

import serial
import re

# 配置串口链接
ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)

# 这里调用re中的compile函数，用于编译正则表达式
# 字符串前的r表示生字符串，里面的反斜杠不会被视为转义
# \d 表示数字字符（0-9）
pattern = re.compile(r'Temperature: ([\d.]+) C, ([\d.]+) F')

while True:
    # 读取一行数据 转换为字符串 去除字符串首尾空白字符
    line = ser.readline().decode('utf-8').strip()
    
    if line:
        # 寻找是否有匹配项
        match = pattern.search(line)
        
        if match:
            # 如果找到匹配项，使用 match.group(1) 和 match.group(2) 提取摄氏度和华氏度的值。
            temp_celsius = match.group(1)
            temp_fahrenheit = match.group(2)
            print(f"Temperature: {temp_celsius} °C, {temp_fahrenheit} °F")