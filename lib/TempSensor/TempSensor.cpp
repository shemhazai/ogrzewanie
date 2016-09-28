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
  dallasTemperature->setWaitForConversion(false);
  dallasTemperature->setResolution(12);
}

TempSensor::~TempSensor() {
  delete dallasTemperature;
  delete oneWire;
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

void TempSensor::diagnose(void (*errorShutdown)()) {
  float tz = readTZ();
  float tkw = readTKW();
  float tco = readTCO();
  float tb = readTB();
  float tcwu = readTCWU();
  float tp = readTP();

  bool isTZWorking = isTZInRange(tz);
  bool isTKWWorking = isTKWInRange(tkw);
  bool isTCOWorking = isTCOInRange(tco);
  bool isTBWorking = isTBInRange(tb);
  bool isTCWUWorking = isTCWUInRange(tcwu);
  bool isTPWorking = isTPInRange(tp);

  bool working = isTZWorking && isTKWWorking && isTCOWorking && isTBWorking &&
                 isTBWorking && isTCWUWorking && isTPWorking;

  if (working) {
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
  } else {
    errorShutdown();

    char text[84];
    sprintf(text, "Tcwu: %c  Tz: %c\n"
                  "Tkw : %c  Tb: %c\n"
                  "Tco : %c  Tp: %c",
            (isTCWUWorking ? '+' : '-'), (isTZWorking ? '+' : '-'),
            (isTKWWorking ? '+' : '-'), (isTBWorking ? '+' : '-'),
            (isTCOWorking ? '+' : '-'), (isTPWorking ? '+' : '-'));
    tempSensorError(text);
  }
}

void TempSensor::tempSensorError(const char *msg) {
  lcd->clear();
  lcd->setCursor(0, 0);

  const uint8_t len = strlen(msg);
  uint8_t line = 0;
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

void TempSensor::copyAddress(const uint8_t src[], uint8_t dest[]) {
  const uint8_t ADDRESS_LEN = 8;
  for (uint8_t i = 0; i < ADDRESS_LEN; i++)
    dest[i] = src[i];
}

void TempSensor::doNothing() {
  while (true)
    delay(1000);
}
