#ifndef MAX6675_H_
#define MAX6675_H_

#include <Arduino.h>

class MAX6675 {
public:
  MAX6675(int8_t SCLK, int8_t CS, int8_t MISO);
  float readCelsius();

private:
  int8_t sclk, miso, cs;
  uint8_t spiread();
};

#endif
