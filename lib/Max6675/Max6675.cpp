#include "Max6675.h"

MAX6675::MAX6675(int8_t SCLK, int8_t CS, int8_t MISO) {
  sclk = SCLK;
  cs = CS;
  miso = MISO;

  pinMode(cs, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);
}

float MAX6675::readCelsius() {
  uint16_t value;

  digitalWrite(cs, LOW);
  delay(1);

  value = spiread();
  value <<= 8;
  value |= spiread();

  digitalWrite(cs, HIGH);

  if (value & 0x4)
    return -999; // no thermocouple attached

  value >>= 3;

  return value * 0.25;
}

byte MAX6675::spiread() {
  int i;
  byte data = 0;

  const int LEN = 7;
  for (i = LEN; i >= 0; i--) {
    digitalWrite(sclk, LOW);
    delay(1);
    if (digitalRead(miso))
      data |= (1 << i);
    digitalWrite(sclk, HIGH);
    delay(1);
  }

  return data;
}
