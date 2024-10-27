import serial
import matplotlib.pyplot as plt
import numpy as np
import time

# 串口配置
ser = serial.Serial(
    port='COM5',     # 根据实际连接的串口修改
    baudrate=9600,   # MSP430 端设置的波特率
    timeout=1        # 超时时间
)

# 定义一个函数读取串口数据
def read_adc_data():
    max_val = None
    min_val = None
    avg_val = None
    
    while True:
        line = ser.readline().decode().strip()
        if line.startswith("Max:"):
            max_val = int(line.split(":")[1].strip())
        elif line.startswith("Min:"):
            min_val = int(line.split(":")[1].strip())
        elif line.startswith("Average:"):
            avg_val = float(line.split(":")[1].strip())

        if max_val is not None and min_val is not None and avg_val is not None:
            return max_val, min_val, avg_val

# 实时绘制波形图
plt.ion()  # 打开交互模式
fig, ax = plt.subplots()
x = np.arange(0, 50, 1)  # 假设每次传输 50 个采样点
y = np.zeros(50)
line, = ax.plot(x, y)

try:
    while True:
        max_val, min_val, avg_val = read_adc_data()
        print(f"Max: {max_val}, Min: {min_val}, Average: {avg_val}")

        # 模拟波形数据（这里只是作为示例，你可以根据需要修改）
        new_data = np.random.randint(min_val, max_val, 50)  # 用随机数据表示

        # 更新波形
        line.set_ydata(new_data)
        ax.relim()
        ax.autoscale_view()

        # 更新图形
        plt.draw()
        plt.pause(0.1)

except KeyboardInterrupt:
    ser.close()
    plt.close()
