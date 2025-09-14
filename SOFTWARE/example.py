from esp32_usb_lib import ESP32USB

esp = ESP32USB(port="/dev/ttyACM0")

esp.setpin(5, 1)          # Set GPIO 5 HIGH
val = esp.readpin(5)      # Read GPIO 5 digital
adc = esp.analogread(4)   # Read analog from GPIO 4
esp.setpwm(6, 128)        # PWM 50% duty on GPIO 6
esp.i2c_write(0x3C, 0x55) # Write byte
data = esp.i2c_read(0x3C, 4)

esp.close()

