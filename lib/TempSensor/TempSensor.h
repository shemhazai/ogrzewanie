#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

class TempSensor {
public:
  TempSensor(uint8_t tempSensorPin, LiquidCrystal *aLcd, uint8_t aBuzzerPin);
  ~TempSensor();

  void requestTemperatures();

  float readTKW();
  float readTB();
  float readTCWU();

  float readTKWWIthoutRequest();
  float readTBWithoutRequest();
  float readTCWUWithoutRequest();

private:
  void tempSensorError(const char *msg);
  void doNothing();

  LiquidCrystal *lcd;
  uint8_t buzzerPin;

  DallasTemperature *dallasTemperature;
  OneWire *oneWire;

  DeviceAddress TKWAddress;
  DeviceAddress TBAddress;
  DeviceAddress TCWUAddress;
};

#endif
