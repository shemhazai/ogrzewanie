#include "TempSensor.h"

TempSensor::TempSensor(uint8_t tempSensorPin) {
  dallasTemperature = new DallasTemperature();
  oneWire = new OneWire(tempSensorPin);
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
