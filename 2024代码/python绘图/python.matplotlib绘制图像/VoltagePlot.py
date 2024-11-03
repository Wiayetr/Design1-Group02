import matplotlib.pyplot as plt
import serial
import re

# 初始化数据存储
ax = []
ay_max = []
ay_min = []
ay_avg = []
plt.ion()

# 设置串口参数
ser = serial.Serial(port="COM5", bytesize=8, baudrate=9600, stopbits=1, timeout=1)
x = 0
plt.figure(figsize=(13, 6))

while True:
    com_input = ser.readline().decode('utf-8').strip()
    if com_input:
        # 使用正则表达式提取最大值、最小值和平均值
        max_pattern = re.compile(r"Max: (\d+)")
        min_pattern = re.compile(r"Min: (\d+)")
        avg_pattern = re.compile(r"Average: ([\d.]+)")

        max_match = max_pattern.search(com_input)
        min_match = min_pattern.search(com_input)
        avg_match = avg_pattern.search(com_input)

        if max_match:
            max_value = float(max_match.group(1))
            ay_max.append(max_value)
            print(f"Max Voltage: {max_value}")

        if min_match:
            min_value = float(min_match.group(1))
            ay_min.append(min_value)
            print(f"Min Voltage: {min_value}")

        if avg_match:
            avg_value = float(avg_match.group(1))
            ay_avg.append(avg_value)
            print(f"Average Voltage: {avg_value}")

            ax.append(x)
            x += 1  # 增加时间轴

            # 清除当前图像
            plt.clf()
            plt.plot(ax, ay_max, label='Max Voltage', marker='o', linestyle='-')
            plt.plot(ax, ay_min, label='Min Voltage', marker='x', linestyle='--')
            plt.plot(ax, ay_avg, label='Average Voltage', marker='s', linestyle=':')
            plt.xlabel("Time (s)")
            plt.ylabel("Voltage (V)")
            plt.title("Real-Time Voltage Monitoring")
            plt.legend()
            plt.grid(True)

            # 更新图像
            plt.pause(0.1)

plt.ioff()
