#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TempSensor.h>
#include <Timer.h>

LiquidCrystal *lcd;
TempSensor *tempSensor;
Timer timer;
float tkw, tb, tcwu;

void initPins();
void initLcd();
void initTempSensor();

void timerTemperatureTask();
void readTemperatures();

int main() {
  initPins();
  initLcd();
  initTempSensor();

  int8_t slotId = timer.setInterval(timerTemperatureTask, 1000);
  while (true) {
    delay(100);
    timer.update();
  }

  lcd->clear();
  lcd->print(sizeof(Timer));

  return 0;
}

void initPins() { pinMode(BUZZER_PIN, OUTPUT); }

void initLcd() {
  lcd = new LiquidCrystal(RS, ENABLE, D4, D5, D6, D7);
  lcd->begin(16, 2);
  lcd->clear();
  lcd->noBlink();
  lcd->noCursor();
}

void initTempSensor() {
  tempSensor = new TempSensor(TEMP_SENSOR_PIN, lcd, BUZZER_PIN);
}

void timerTemperatureTask() {
  readTemperatures();
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print(tkw);
  lcd->setCursor(5, 0);
  lcd->print(tb);
  lcd->setCursor(0, 1);
  lcd->print(tcwu);
}

void readTemperatures() {
  tempSensor->requestTemperatures();
  tkw = tempSensor->readTKWWIthoutRequest();
  tb = tempSensor->readTBWithoutRequest();
  tcwu = tempSensor->readTCWUWithoutRequest();
}
