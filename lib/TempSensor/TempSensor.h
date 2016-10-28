#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <DallasTemperature.h>
#include <Max6675.h>
#include <OneWire.h>

class TempSensor {
public:
  TempSensor(uint8_t tempSensorPin);
  ~TempSensor();

  void setMAX6675Pins(uint8_t sclk, uint8_t cs, uint8_t miso);

  void requestTemperatures();

  float readTZ();
  float readTKW();
  float readTCO();
  float readTB();
  float readTCWU();
  float readTP();
  float readTSKW();
  float readTW();

  bool isTZInRange(const float temp);
  bool isTKWInRange(const float temp);
  bool isTCOInRange(const float temp);
  bool isTBInRange(const float temp);
  bool isTCWUInRange(const float temp);
  bool isTPInRange(const float temp);
  bool isTSKWInRange(const float temp);
  bool isTWInRange(const float temp);

  void setTZAddress(const uint8_t aTZAddress[]);
  void setTKWAddress(const uint8_t aTKWAddress[]);
  void setTCOAddress(const uint8_t aTCOAddress[]);
  void setTBAddress(const uint8_t aTBAddress[]);
  void setTCWUAddress(const uint8_t aTCWUAddress[]);
  void setTPAddress(const uint8_t aTPAddress[]);
  void setTWAddress(const uint8_t aTWAddress[]);

private:
  void copyAddress(const uint8_t src[], uint8_t dest[]);

  DallasTemperature *dallasTemperature;
  OneWire *oneWire;

  MAX6675 *thermocouple;

  uint8_t TZAddress[8];
  uint8_t TKWAddress[8];
  uint8_t TCOAddress[8];
  uint8_t TBAddress[8];
  uint8_t TCWUAddress[8];
  uint8_t TPAddress[8];
  uint8_t TWAddress[8];
};

#endif
