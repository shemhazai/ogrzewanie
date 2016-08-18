#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <DallasTemperature.h>
#include <OneWire.h>

class TempSensor {
public:
  TempSensor(uint8_t tempSensorPin);
  ~TempSensor();
  /*
    functions to read temperature
    float readATemperature(); // example
  */
private:
  DallasTemperature *dallasTemperature;
  OneWire *oneWire;
  // device addresses
};

#endif
