import matplotlib.pyplot as plt
import serial
import re

ax = []
ay = []
ay_fah = []
plt.ion()
ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)
x = 0
plt.figure(figsize=(13, 6))

# 创建两个子图
ax1 = plt.subplot(2, 1, 1)  # 上部图
ax2 = plt.subplot(2, 1, 2)  # 下部图

# 初始绘制
line_celsius, = ax1.plot([], [], marker='o', linestyle='-')
line_fahrenheit, = ax2.plot([], [], marker='x', linestyle='--')

# 设置图表标题和标签
ax1.set_xlabel("Time (s)")
ax1.set_ylabel("Temperature (°C)")
ax1.set_title("Celsius Temperature Monitoring")
ax1.grid(True)

ax2.set_xlabel("Time (s)")
ax2.set_ylabel("Temperature (°F)")
ax2.set_title("Fahrenheit Temperature Monitoring")
ax2.grid(True)

while True:
    com_input = ser.readline().decode('utf-8').strip()
    if com_input:
        # 使用正则表达式提取摄氏度和华氏度
        pattern = re.compile(r'Temperature: ([\d.]+) C, ([\d.]+) F')
        match = pattern.search(com_input)
        
        if match:
            temp_celsius = float(match.group(1))
            temp_fahrenheit = float(match.group(2))

            ax.append(x)
            ay.append(temp_celsius)
            ay_fah.append(temp_fahrenheit)
            x += 0.2
            
            # 更新数据
            line_celsius.set_data(ax, ay)
            line_fahrenheit.set_data(ax, ay_fah)
            
            # 设置坐标轴范围
            ax1.set_xlim(min(ax), max(ax))
            ax1.set_ylim(min(ay) - 1, max(ay) + 1)
            ax2.set_xlim(min(ax), max(ax))
            ax2.set_ylim(min(ay_fah) - 1, max(ay_fah) + 1)

            # 更新绘图
            plt.tight_layout()
            plt.pause(0.1)

plt.ioff()
