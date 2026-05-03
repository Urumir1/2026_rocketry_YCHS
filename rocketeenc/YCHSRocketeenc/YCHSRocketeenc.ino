#include "Wire.h"

#define BUZZPIN 4
#define DEVMODE 1
#define I2CADDR 0x26

int ZACCELPIN = A6;

int zAxis = 0;
int yAxis = 0;
int xAxis = 0;

void setup() {
  Wire.begin(I2CADDR); //Start in I2C Slave mode.
  Wire.onReceive(receiveI2C);
  Wire.onRequest(requestI2C);

  pinMode(BUZZPIN, OUTPUT);
  xAxis = analogRead(A6);
  yAxis = analogRead(A7);
  zAxis = analogRead(A8);
  if (DEVMODE) {
    Serial.begin(9600);
    Serial.printf("Z: %i, Y: %i, X: %i\n", zAxis, yAxis, xAxis);
  }
  tone(BUZZPIN, xAxis, 200);
  delay(500);
  tone(BUZZPIN, yAxis, 200);
  delay(500);
  tone(BUZZPIN, zAxis, 200);
}

void receiveI2C(int count) {
  while (Wire.available()) {
    char c = Wire.read();
    Serial.printf("I2C receive %c\n", c);
  }
  tone(BUZZPIN, 2048, 200);
}

void requestI2C() {
  Wire.write("TeenC");

}

void loop() {
 delay(100);

}
