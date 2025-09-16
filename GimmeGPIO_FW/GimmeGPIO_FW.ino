#include <Arduino.h>
#include <Wire.h>

// Hardware UART for passthrough
#define HW_UART_RX 15  // to default pins on RPI style header
#define HW_UART_TX 14  // to default pins on RPI style header
HardwareSerial HWSerial(1);

// I2C pins
#define I2C_SDA 2  // change as needed
#define I2C_SCL 3  // change as needed

bool uartPassthrough = false;

void setup() {
  Serial.begin(115200); // USB CDC serial
  HWSerial.begin(115200, SERIAL_8N1, HW_UART_RX, HW_UART_TX);
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("GimmeGPIO ready");
}

void loop() {
  // Handle USB -> commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    parseCommand(cmd);
  }

  // Handle UART passthrough if enabled
  if (uartPassthrough) {
    while (Serial.available()) {
      HWSerial.write(Serial.read());
    }
    while (HWSerial.available()) {
      Serial.write(HWSerial.read());
    }
  }
}

void parseCommand(String cmd) {
  if (cmd.startsWith("setpin")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(',');
    int p3 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1 && p3 > p2) {
      int pin = cmd.substring(p1 + 1, p2).toInt();
      int state = cmd.substring(p2 + 1, p3).toInt();
      pinMode(pin, OUTPUT);
      digitalWrite(pin, state ? HIGH : LOW);
      Serial.printf("Pin %d set to %d\n", pin, state);
    }
  } else if (cmd.startsWith("setpwm")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(',');
    int p3 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1 && p3 > p2) {
      int pin = cmd.substring(p1 + 1, p2).toInt();
      int pwm = cmd.substring(p2 + 1, p3).toInt();
      //ledcSetup(pin, 5000, 8); //old way
     // ledcAttachPin(pin, pin); // use pin number as channel for simplicity
      ledcAttach(pin,5000,8);
      ledcWrite(pin, pwm);
      Serial.printf("PWM set on pin %d: %d\n", pin, pwm);
    }
  } else if (cmd.startsWith("readpin")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1) {
      int pin = cmd.substring(p1 + 1, p2).toInt();
      pinMode(pin, INPUT);
      int value = digitalRead(pin);
      Serial.printf("Pin %d reads %d\n", pin, value);
    }
  } else if (cmd.startsWith("analogread")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1) {
      int pin = cmd.substring(p1 + 1, p2).toInt();
      int value = analogRead(pin);
      Serial.printf("Analog read pin %d: %d\n", pin, value);
    }
  } else if (cmd.startsWith("UART")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1) {
      int state = cmd.substring(p1 + 1, p2).toInt();
      uartPassthrough = (state == 1);
      Serial.printf("UART passthrough %s\n", uartPassthrough ? "enabled" : "disabled");
    }
  } else if (cmd.startsWith("I2C(write")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(',');
    int p3 = cmd.indexOf(',', p2 + 1);
    int p4 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1 && p3 > p2 && p4 > p3) {
      int addr = cmd.substring(p1 + 1, p2).toInt();
      int data = cmd.substring(p3 + 1, p4).toInt();
      Wire.beginTransmission(addr);
      Wire.write((uint8_t)data);
      Wire.endTransmission();
      Serial.printf("I2C write to 0x%02X: 0x%02X\n", addr, data);
    }
  } else if (cmd.startsWith("I2C(read")) {
    int p1 = cmd.indexOf('(');
    int p2 = cmd.indexOf(',');
    int p3 = cmd.indexOf(')');
    if (p1 > 0 && p2 > p1 && p3 > p2) {
      int addr = cmd.substring(p1 + 1, p2).toInt();
      int len = cmd.substring(p2 + 1, p3).toInt();
      Wire.requestFrom(addr, len);
      Serial.printf("I2C read from 0x%02X: ", addr);
      while (Wire.available()) {
        uint8_t b = Wire.read();
        Serial.printf("0x%02X ", b);
      }
      Serial.println();
    }
  } else {
    Serial.println("Unknown command");
  }
}
