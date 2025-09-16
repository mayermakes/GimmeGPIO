from esp32_usb_lib import ESP32USB
import time

esp = ESP32USB(port="/dev/ttyACM0")
while True:
	esp.setpin(2,0)
	time.sleep(3)
	esp.setpin(2, 1)  
	        
	val = esp.readpin(2)
	print("digital value: "+ str(val))      
	time.sleep(2)
	print("set PWM to 40")
	esp.setpwm(2,40) 
	time.sleep(2)
	print("set PWM to 80")
	esp.setpwm(2, 80) 
	time.sleep(2)
	print("set PWM to 160")
	esp.setpwm(2, 160) 
	time.sleep(2)
	print("set PWM to 255")
	esp.setpwm(2, 255) 
	time.sleep(3)  
	adc = esp.analogread(2)
	print("analog value: "+ str(adc))     
	time.sleep(1)
	esp.setpwm(2, 100)
	time.sleep(3)
	byte=0x55
	esp.i2c_write(0x3C, byte) 
	print("Byte written: " + str(byte))
	data = esp.i2c_read(0x3C, 4)
	print("Byte read back: " + str(data))
	time.sleep(2)
	



esp.close()

