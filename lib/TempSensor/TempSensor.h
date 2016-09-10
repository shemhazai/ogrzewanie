#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

class TempSensor {
public:
  TempSensor(uint8_t tempSensorPin, LiquidCrystal *aLcd, uint8_t aBuzzerPin,
             uint8_t aTskwPin);
  ~TempSensor();

  void requestTemperatures();

  float readTZ();
  float readTKW();
  float readTSKW();
  float readTCO();
  float readTB();
  float readTCWU();

  float readTZWithoutRequest();
  float readTKWWIthoutRequest();
  float readTCOWithoutRequest();
  float readTBWithoutRequest();
  float readTCWUWithoutRequest();

  bool isTZInRange(const float temp);
  bool isTKWInRange(const float temp);
  bool isTSKWInRange(const float temp);
  bool isTCOInRange(const float temp);
  bool isTBInRange(const float temp);
  bool isTCWUInRange(const float temp);

  void setTZAddress(const uint8_t aTZAddress[]);
  void setTKWAddress(const uint8_t aTKWAddress[]);
  void setTCOAddress(const uint8_t aTCOAddress[]);
  void setTBAddress(const uint8_t aTBAddress[]);
  void setTCWUAddress(const uint8_t aTCWUAddress[]);

  void tempSensorError(const char *msg);

private:
  void doNothing();

  LiquidCrystal *lcd;
  uint8_t buzzerPin;
  uint8_t tskwPin;

  DallasTemperature *dallasTemperature;
  OneWire *oneWire;

  uint8_t TZAddress[8];
  uint8_t TKWAddress[8];
  uint8_t TCOAddress[8];
  uint8_t TBAddress[8];
  uint8_t TCWUAddress[8];
};

#endif
