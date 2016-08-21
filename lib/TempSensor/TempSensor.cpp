#include "TempSensor.h"
#include <Arduino.h>

TempSensor::TempSensor(uint8_t tempSensorPin, LiquidCrystal *aLcd,
                       uint8_t aBuzzerPin) {
  lcd = aLcd;
  buzzerPin = aBuzzerPin;

  dallasTemperature = new DallasTemperature();
  oneWire = new OneWire(tempSensorPin);
  dallasTemperature->setOneWire(oneWire);
  dallasTemperature->begin();

  if (!dallasTemperature->getAddress(TKWAddress, 0))
    tempSensorError("Blad czujnika TKW!");
  if (!dallasTemperature->getAddress(TBAddress, 1))
    tempSensorError("Blad czujnika TB!");
  if (!dallasTemperature->getAddress(TCWUAddress, 2))
    tempSensorError("Blad czujnika TCWU!");

  dallasTemperature->setResolution(9);
}

TempSensor::~TempSensor() {
  delete dallasTemperature;
  delete oneWire;
}

void TempSensor::requestTemperatures() {
  dallasTemperature->requestTemperatures();
}

float TempSensor::readTKW() {
  dallasTemperature->requestTemperaturesByAddress(TKWAddress);
  return dallasTemperature->getTempC(TKWAddress);
}

float TempSensor::readTB() {
  dallasTemperature->requestTemperaturesByAddress(TBAddress);
  return dallasTemperature->getTempC(TBAddress);
}

float TempSensor::readTCWU() {
  dallasTemperature->requestTemperaturesByAddress(TCWUAddress);
  return dallasTemperature->getTempC(TCWUAddress);
}

float TempSensor::readTKWWIthoutRequest() {
  return dallasTemperature->getTempC(TKWAddress);
}

float TempSensor::readTBWithoutRequest() {
  return dallasTemperature->getTempC(TBAddress);
}

float TempSensor::readTCWUWithoutRequest() {
  return dallasTemperature->getTempC(TCWUAddress);
}

void TempSensor::tempSensorError(const char *msg) {
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print(msg);

  digitalWrite(buzzerPin, HIGH);
  doNothing();
}

void TempSensor::doNothing() {
  while (true)
    delay(1000);
}
