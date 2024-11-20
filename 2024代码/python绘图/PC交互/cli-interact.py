import serial
import time
from TemperaturePlot import TemperaturePlot

# Configure the serial port
ser = serial.Serial(
    port='COM5',  # Replace with your actual COM port
    baudrate=9600,
    timeout=1
)

def send_command(command):
    """Send a command to the MSP430."""
    ser.reset_input_buffer()  # Clear any lingering input data
    ser.reset_output_buffer()  # Clear any lingering output data
    ser.write(command.encode())
    time.sleep(0.1)  # Small delay to allow MSP430 to process the command

def read_from_msp430():
    """Continuously read data from the MSP430."""
    try:
        while True:
            if ser.in_waiting > 0:
                response = ser.readline().decode().strip()
                print(f"MSP430: {response}")
            time.sleep(0.1)  # Avoid busy-waiting
    except KeyboardInterrupt:
        print("Exiting data reading.")

def temperature_read(ser):
    """Continuously read data from the MSP430."""
    try:
        TemperaturePlot(ser)
    except KeyboardInterrupt:
        print("Exiting data reading.")

def main():
    try:
        while True:
            command = input("Enter command (start_voltage, start_temp, stop): ").strip()

            if command == 'start_voltage':
                send_command('V')  # Command to start voltage measurement
                print("Starting voltage measurement. Press Ctrl+C to stop.")
                read_from_msp430()  # Enter reading loop
            elif command == 'start_temp':
                send_command('T')  # Command to start temperature measurement
                print("Starting temperature measurement. Press Ctrl+C to stop.")
                temperature_read(ser)
            elif command == 'stop':
                send_command('S')  # Command to stop measurement
                print("Measurement stopped.")
            else:
                print("Invalid command. Try again.")

    except KeyboardInterrupt:
        print("Exiting program.")
    finally:
        # Ensure proper cleanup of the serial port
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        ser.close()
        print("Serial port closed.")

if __name__ == "__main__":
    main()