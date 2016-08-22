#include "config.h"
#include <Arduino.h>
#include <DateTime.h>
#include <LiquidCrystal.h>
#include <RealTimeClock.h>
#include <TempSensor.h>
#include <Timer.h>
#include <math.h>

LiquidCrystal *lcd;
TempSensor *tempSensor;
RealTimeClock rtc(SDA, SCL);
Timer timer;
float tkw, tb, tcwu;

void initPins();
void initLcd();
void initTempSensor();

void readAndDisplayTemperatures();
void tempSensorError(const char *name, const float temp);
void displayTime();

int main() {
  initPins();
  initLcd();
  initTempSensor();

  timer.setInterval(readAndDisplayTemperatures, 1000);
  timer.setInterval(displayTime, 1000);
  while (true) {
    delay(10);
    timer.update();
  }

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

void readAndDisplayTemperatures() {
  tkw = tempSensor->readTKW();
  tb = tempSensor->readTB();
  tcwu = tempSensor->readTCWU();

  if (!tempSensor->isInRange(tkw))
    tempSensorError("TKW", tkw);
  if (!tempSensor->isInRange(tb))
    tempSensorError("TB", tb);
  if (!tempSensor->isInRange(tcwu))
    tempSensorError("TCWU", tcwu);

  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print(tkw);
  lcd->setCursor(6, 0);
  lcd->print(tb);
  lcd->setCursor(12, 0);
  lcd->print(tcwu);
}

void tempSensorError(const char *name, const float temp) {
  char text[40];
  sprintf(text, "Bledna temperatura!\n%s: %d.%d", name, (int)temp,
          abs((int)(temp * 10) % 10));
  tempSensor->tempSensorError(text);
}

void displayTime() {
  DateTime dt = rtc.getDateTime();

  char text[17];
  for (int i = 0; i < 16; i++)
    text[i] = ' ';
  text[16] = 0;
  lcd->setCursor(0, 1);
  lcd->print(text);

  sprintf(text, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
  lcd->setCursor(0, 1);
  lcd->print(text);
}
