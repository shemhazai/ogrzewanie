#include "TempSensor.h"
#include <Arduino.h>

TempSensor::TempSensor(uint8_t tempSensorPin, LiquidCrystal *aLcd,
                       uint8_t aBuzzerPin, uint8_t aTskwPin) {
  lcd = aLcd;
  buzzerPin = aBuzzerPin;
  tskwPin = aTskwPin;

  dallasTemperature = new DallasTemperature();
  oneWire = new OneWire(tempSensorPin);
  dallasTemperature->setOneWire(oneWire);
  dallasTemperature->begin();
  dallasTemperature->setResolution(9);
}

TempSensor::~TempSensor() {
  delete dallasTemperature;
  delete oneWire;
}

float TempSensor::readTZ() {
  dallasTemperature->requestTemperaturesByAddress(TZAddress);
  return dallasTemperature->getTempC(TZAddress);
}

float TempSensor::readTKW() {
  dallasTemperature->requestTemperaturesByAddress(TKWAddress);
  return dallasTemperature->getTempC(TKWAddress);
}

float TempSensor::readTSKW() {
  const double R0 = 500;
  const double I = 2.73;
  const double A = 3.90802e-3;
  const double B = -5.802e-7;

  int sum = analogRead(tskwPin);
  for (int i = 0; i < 4; i++) {
    delay(1);
    sum += analogRead(tskwPin);
  }

  double U = sum / 5115.0 * 5000.0;
  double R1 = U / I;
  double delta = (R0 * R0) * (A * A) - (4 * R0 * B * (R0 - R1));
  double T = (-R0 * A + sqrt(delta)) / (2 * R0 * B);
  return (float)T;
}

float TempSensor::readTCO() {
  dallasTemperature->requestTemperaturesByAddress(TCOAddress);
  return dallasTemperature->getTempC(TCOAddress);
}

float TempSensor::readTB() {
  dallasTemperature->requestTemperaturesByAddress(TBAddress);
  return dallasTemperature->getTempC(TBAddress);
}

float TempSensor::readTCWU() {
  dallasTemperature->requestTemperaturesByAddress(TCWUAddress);
  return dallasTemperature->getTempC(TCWUAddress);
}

float TempSensor::readTP() {
  dallasTemperature->requestTemperaturesByAddress(TPAddress);
  return dallasTemperature->getTempC(TPAddress);
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

bool TempSensor::isTSKWInRange(const float temp) {
  const float MAX_TEMP = 600;
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

void TempSensor::setTZAddress(const uint8_t aTZAddress[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    TZAddress[i] = aTZAddress[i];
}

void TempSensor::setTKWAddress(const uint8_t aTKWAddress[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    TKWAddress[i] = aTKWAddress[i];
}

void TempSensor::setTCOAddress(const uint8_t aTCOAddress[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    TCOAddress[i] = aTCOAddress[i];
}

void TempSensor::setTBAddress(const uint8_t aTBAddress[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    TBAddress[i] = aTBAddress[i];
}

void TempSensor::setTCWUAddress(const uint8_t aTCWUAddress[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    TCWUAddress[i] = aTCWUAddress[i];
}

void TempSensor::setTPAddress(const uint8_t aTPAddress[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    TPAddress[i] = aTPAddress[i];
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
