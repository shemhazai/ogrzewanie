#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <DallasTemperature.h>
#include <OneWire.h>

class TempSensor {
public:
  TempSensor();
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
