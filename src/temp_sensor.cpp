#include "config.h"
#include "temp_sensor.h"

TempSensor::TempSensor() {
  dallasTemperature = new DallasTemperature();
  oneWire = new OneWire(TEMP_SENSOR_PIN);
  dallasTemperature->setOneWire(oneWire);
  dallasTemperature->begin();
  // setup sensors
}

TempSensor::~TempSensor() {
  delete dallasTemperature;
  delete oneWire;
}

/*
  float TempSensor::readATemperature() {
    dallasTemperature.requestTemperaturesByAddress(address);
    return dallasTemperature.getTempC(address);
  }
*/
