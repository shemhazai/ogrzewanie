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

bool TempSensor::isInRange(const float temp) {
  const int MIN_TEMP = 0;
  const int MAX_TEMP = 120;
  return temp >= MIN_TEMP && temp <= MAX_TEMP;
}

void TempSensor::tempSensorError(const char *msg) {
  lcd->clear();
  lcd->setCursor(0, 0);

  const uint8_t len = strlen(msg);
  uint8_t line = 0;
  lcd->setCursor(0, line);
  for (uint8_t i = 0; i < len; i++) {
    if (msg[i] == '\n') {
      line++;
      lcd->setCursor(0, line);
    } else {
      lcd->print(msg[i]);
    }
  }

  digitalWrite(buzzerPin, HIGH);
  doNothing();
}

void TempSensor::doNothing() {
  while (true)
    delay(1000);
}
