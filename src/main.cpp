#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TempSensor.h>
#include <Timer.h>

LiquidCrystal *lcd;
TempSensor *tempSensor;
Timer timer;

float tz, tkw, tskw, tco, tb, tcwu, tkg;

void initPins();
void initLcd();
void initTempSensor();

void readTemperatures();
void prepareAndReadTSKW();
void readTSKW();
void readTKG();
void controlZT();
void tempSensorError(const char *name, const float temp);
void updateDisplay();

bool shouldTurnOnPKW() { return tkw > 40 && tskw > 100; }
bool shouldTurnOffPKW() { return tskw < 100; }
bool shouldTurnOnPCWU() { return (tb > tcwu + 5) && (tcwu < 60); }

int main() {
  initPins();
  initLcd();
  initTempSensor();

  timer.setInterval(readTemperatures, 2000);
  timer.setInterval(prepareAndReadTSKW, 5000);
  timer.setInterval(readTKG, 60000);
  timer.setInterval(controlZT, 10000); // min 5.1s
  timer.setInterval(updateDisplay, 1000);

  readTemperatures();
  prepareAndReadTSKW();
  readTKG();
  updateDisplay();

  while (true) {
    if (shouldTurnOnPKW()) {
      digitalWrite(PKW_PIN, HIGH);
    } else if (shouldTurnOffPKW()) {
      digitalWrite(PKW_PIN, LOW);
    }

    if (shouldTurnOnPCWU()) {
      digitalWrite(PCWU_PIN, HIGH);
    } else {
      digitalWrite(PCWU_PIN, LOW);
    }

    timer.update();
  }

  return 0;
}

void initPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TSKW_POWER_PIN, OUTPUT);
  pinMode(PKW_PIN, OUTPUT);
  pinMode(PCWU_PIN, OUTPUT);
  pinMode(ZTZ_PIN, OUTPUT);
  pinMode(ZTC_PIN, OUTPUT);
}

void initLcd() {
  lcd = new LiquidCrystal(RS, ENABLE, D4, D5, D6, D7);
  lcd->begin(16, 2);
  lcd->clear();
  lcd->noBlink();
  lcd->noCursor();
}

void initTempSensor() {
  tempSensor = new TempSensor(TEMP_SENSOR_PIN, lcd, BUZZER_PIN, TSKW_PIN);
  tempSensor->setTZAddress(TZAddress);
  tempSensor->setTKWAddress(TKWAddress);
  tempSensor->setTCOAddress(TCOAddress);
  tempSensor->setTBAddress(TBAddress);
  tempSensor->setTCWUAddress(TCWUAddress);
}

void readTemperatures() {
  tz = tempSensor->readTZ();
  tkw = tempSensor->readTKW();
  tco = tempSensor->readTCO();
  tb = tempSensor->readTB();
  tcwu = tempSensor->readTCWU();

  if (!tempSensor->isTZInRange(tz))
    tempSensorError("TZ", tz);
  if (!tempSensor->isTKWInRange(tkw))
    tempSensorError("TKW", tkw);
  if (!tempSensor->isTCOInRange(tco))
    tempSensorError("TCO", tco);
  if (!tempSensor->isTBInRange(tb))
    tempSensorError("TB", tb);
  if (!tempSensor->isTCWUInRange(tcwu))
    tempSensorError("TCWU", tcwu);
}

void prepareAndReadTSKW() {
  digitalWrite(TSKW_POWER_PIN, HIGH);
  const int POWER_ON_TIME = 500;
  timer.setTimeout(readTSKW, POWER_ON_TIME);
}

void readTSKW() {
  tskw = tempSensor->readTSKW();
  if (!tempSensor->isTSKWInRange(tskw))
    tempSensorError("TSKW", tskw);
  digitalWrite(TSKW_POWER_PIN, LOW);
}

void readTKG() {
  const double KG = 1.1;  // nachylenie krzywej grzania
  const double TW = 21.5; // pożądana temperatura wewnątrz
  tkg = KG * (TW - tz) + TW;
}

void openZTCEnd() { digitalWrite(ZTC_PIN, LOW); }
void openZTZEnd() { digitalWrite(ZTZ_PIN, LOW); }

void controlZT() {
  if (tco < tkg) {
    digitalWrite(ZTC_PIN, HIGH);
    timer.setTimeout(openZTCEnd, 3100); // 1 st.
  } else {
    digitalWrite(ZTZ_PIN, HIGH);
    timer.setTimeout(openZTZEnd, 5100); // 5/3 st.
  }
}

void tempSensorError(const char *name, const float temp) {
  timer.deleteAllTimers();

  digitalWrite(PKW_PIN, LOW);
  digitalWrite(PCWU_PIN, LOW);
  digitalWrite(ZTZ_PIN, HIGH);
  digitalWrite(ZTC_PIN, LOW);
  digitalWrite(TSKW_POWER_PIN, LOW);

  char text[40];
  sprintf(text, "Bledna temperatura!\n%s: %d.%d C", name, (int)temp,
          abs((int)(temp * 10) % 10));
  tempSensor->tempSensorError(text);
}

char buff[10];
void formatFloat(const float temp) {
  sprintf(buff, "%d.%d", abs((int)tkw), abs((int)(tkw * 10) % 10));
}

void updateDisplay() {
  lcd->clear();

  lcd->setCursor(0, 0);
  lcd->print("TSKW ");
  lcd->print((int)tskw);

  formatFloat(tkw);
  lcd->setCursor(10, 0);
  lcd->print("TKW ");
  lcd->print(buff);

  formatFloat(tcwu);
  lcd->setCursor(0, 1);
  lcd->print("TCWU ");
  lcd->print(buff);

  formatFloat(tco);
  lcd->setCursor(10, 1);
  lcd->print("TCO ");
  lcd->print(buff);

  formatFloat(tb);
  lcd->setCursor(2, 0);
  lcd->print("TB   ");
  lcd->print(buff);

  formatFloat(tz);
  lcd->setCursor(2, 10);
  lcd->print("TZ ");
  if (tz < 0) {
    lcd->print("-");
  } else {
    lcd->print(" ");
  }
  lcd->print(buff);

  formatFloat(tkg);
  lcd->setCursor(3, 0);
  lcd->print("TKG  ");
  lcd->print(buff);
}
