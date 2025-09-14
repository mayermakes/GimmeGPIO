import serial
import time

class ESP32USB:
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 1):
        self.ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
        time.sleep(2)  # allow device reset after connect

    def _send_command(self, cmd: str) -> str:
        self.ser.reset_input_buffer()
        self.ser.write((cmd + "\n").encode())
        self.ser.flush()
        response = self.ser.readline().decode(errors="ignore").strip()
        return response

    def setpin(self, pin: int, state: int) -> str:
        return self._send_command(f"setpin({pin},{state})")

    def setpwm(self, pin: int, pwm: int) -> str:
        return self._send_command(f"setpwm({pin},{pwm})")

    def readpin(self, pin: int) -> int:
        resp = self._send_command(f"readpin({pin})")
        try:
            return int(resp.split()[-1])
        except:
            return -1

    def analogread(self, pin: int) -> int:
        resp = self._send_command(f"analogread({pin})")
        try:
            return int(resp.split()[-1])
        except:
            return -1

    def uart(self, state: int) -> str:
        return self._send_command(f"UART({state})")

    def i2c_write(self, addr: int, data: int) -> str:
        return self._send_command(f"I2C(write,{addr},{data})")

    def i2c_read(self, addr: int, length: int) -> list:
        resp = self._send_command(f"I2C(read,{addr},{length})")
        # Expecting something like: "I2C read from 0x3C: 0xAA 0xBB ..."
        try:
            parts = resp.split(':')[-1].strip().split()
            return [int(x, 16) for x in parts]
        except:
            return []

    def close(self):
        if self.ser.is_open:
            self.ser.close()


# Example usage:
if __name__ == "__main__":
    esp = ESP32USB(port="/dev/ttyACM0")  # change port for your system
    print(esp.setpin(5, 1))
    print(esp.readpin(5))
    print(esp.analogread(4))
    print(esp.setpwm(6, 128))
    print(esp.i2c_write(0x3C, 0x55))
    print(esp.i2c_read(0x3C, 4))
    esp.close()
