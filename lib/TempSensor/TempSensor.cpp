#include "TempSensor.h"
#include <Arduino.h>

TempSensor::TempSensor(uint8_t tempSensorPin) {
  dallasTemperature = new DallasTemperature();
  oneWire = new OneWire(tempSensorPin);
  dallasTemperature->setOneWire(oneWire);
  dallasTemperature->begin();
  dallasTemperature->setWaitForConversion(false);
  dallasTemperature->setResolution(12);

  thermocouple = NULL;
}

TempSensor::~TempSensor() {
  delete dallasTemperature;
  delete oneWire;
  delete thermocouple;
}

void TempSensor::setMAX6675Pins(uint8_t sclk, uint8_t cs, uint8_t miso) {
  if (thermocouple != NULL)
    delete thermocouple;
  thermocouple = new MAX6675(sclk, cs, miso);
}

void TempSensor::requestTemperatures() {
  dallasTemperature->requestTemperatures();
}

float TempSensor::readTZ() {
  float temperature = dallasTemperature->getTempC(TZAddress);
  return temperature;
}

float TempSensor::readTKW() {
  float temperature = dallasTemperature->getTempC(TKWAddress);
  return temperature;
}

float TempSensor::readTCO() {
  float temperature = dallasTemperature->getTempC(TCOAddress);
  return temperature;
}

float TempSensor::readTB() {
  float temperature = dallasTemperature->getTempC(TBAddress);
  return temperature;
}

float TempSensor::readTCWU() {
  float temperature = dallasTemperature->getTempC(TCWUAddress);
  return temperature;
}

float TempSensor::readTP() {
  float temperature = dallasTemperature->getTempC(TPAddress);
  return temperature;
}

float TempSensor::readTSKW() {
  float temperature = thermocouple->readCelsius();
  return temperature;
}

bool TempSensor::isTZInRange(const float temp) {
  const float MAX_TEMP = 50;
  const float MIN_TEMP = -40;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

bool TempSensor::isTKWInRange(const float temp) {
  const float MAX_TEMP = 120;
  const float MIN_TEMP = 0;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

bool TempSensor::isTCOInRange(const float temp) {
  const float MAX_TEMP = 120;
  const float MIN_TEMP = 0;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

bool TempSensor::isTBInRange(const float temp) {
  const float MAX_TEMP = 120;
  const float MIN_TEMP = 0;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

bool TempSensor::isTCWUInRange(const float temp) {
  const float MAX_TEMP = 120;
  const float MIN_TEMP = 0;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

bool TempSensor::isTPInRange(const float temp) {
  const float MAX_TEMP = 120;
  const float MIN_TEMP = 0;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

bool TempSensor::isTSKWInRange(const float temp) {
  if (thermocouple == NULL)
    return -999;

  const float MAX_TEMP = 700;
  const float MIN_TEMP = 0;
  return temp > MIN_TEMP && temp < MAX_TEMP;
}

void TempSensor::setTZAddress(const uint8_t aTZAddress[]) {
  copyAddress(aTZAddress, TZAddress);
}

void TempSensor::setTKWAddress(const uint8_t aTKWAddress[]) {
  copyAddress(aTKWAddress, TKWAddress);
}

void TempSensor::setTCOAddress(const uint8_t aTCOAddress[]) {
  copyAddress(aTCOAddress, TCOAddress);
}

void TempSensor::setTBAddress(const uint8_t aTBAddress[]) {
  copyAddress(aTBAddress, TBAddress);
}

void TempSensor::setTCWUAddress(const uint8_t aTCWUAddress[]) {
  copyAddress(aTCWUAddress, TCWUAddress);
}

void TempSensor::setTPAddress(const uint8_t aTPAddress[]) {
  copyAddress(aTPAddress, TPAddress);
}

void TempSensor::copyAddress(const uint8_t src[], uint8_t dest[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    dest[i] = src[i];
}
