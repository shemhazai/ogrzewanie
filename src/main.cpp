#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TempSensor.h>
#include <Timer.h>

LiquidCrystal *lcd;
TempSensor *tempSensor;
Timer timer;

float tz, tkw, tskw = 500, tco, tb, tcwu, tkg, tp;

void setup();
void loop();

void initPins();
void initLcd();
void initTempSensor();

void readTemperatures();
void computeTKG();
void controlZT();

void stopOnError();

void updateDisplay();
void printTSKW();
void printTKW();
void printTCWU();
void printTCO();
void printTB();
void printTZ();
void printTKG();
void printTP();

inline bool shouldTurnOnPKW() { return tkw >= 40 && tskw >= 100; }
inline bool shouldTurnOffPKW() { return tskw <= 100; }
inline bool shouldTurnOnPCWU() { return (tb >= tcwu + 5) && (tcwu < 63); }
inline bool shouldTurnOffPCWU() { return (tb <= tcwu + 2) || (tcwu >= 65); }
inline bool shouldTurnOnPCO() { return tz < 17; }
inline bool shouldTurnOffPCO() { return tz >= 18; }
inline bool shouldOpenZT() { return tco <= tkg - 1.5; }
inline bool shouldCloseZT() { return tco >= tkg + 1.5; }

int main() {
  setup();
  while (true) {
    loop();
  }

  return 0;
}

void setup() {
  initPins();
  initLcd();
  initTempSensor();

  timer.setInterval(readTemperatures, 2000);
  timer.setInterval(computeTKG, 600000); // 10min.
  timer.setInterval(updateDisplay, 1000);

  readTemperatures();
  computeTKG();
  updateDisplay();
  controlZT(); // recursive function
}

void loop() {
  if (shouldTurnOnPKW()) {
    digitalWrite(PKW_PIN, HIGH);
  } else if (shouldTurnOffPKW()) {
    digitalWrite(PKW_PIN, LOW);
  }

  if (shouldTurnOnPCWU()) {
    digitalWrite(PCWU_PIN, HIGH);
  } else if (shouldTurnOffPCWU()) {
    digitalWrite(PCWU_PIN, LOW);
  }

  if (shouldTurnOnPCO()) {
    digitalWrite(PCO_PIN, HIGH);
  } else if (shouldTurnOffPCO()) {
    digitalWrite(PCO_PIN, LOW);
  }

  timer.update();
}

void initPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PKW_PIN, OUTPUT);
  pinMode(PCWU_PIN, OUTPUT);
  pinMode(PCO_PIN, OUTPUT);
  pinMode(ZTZ_PIN, OUTPUT);
  pinMode(ZTC_PIN, OUTPUT);
}

void initLcd() {
  lcd = new LiquidCrystal(RS, RW, ENABLE, D0, D1, D2, D3, D4, D5, D6, D7);
  lcd->begin(20, 4);
  lcd->clear();
  lcd->noBlink();
  lcd->noCursor();
}

void initTempSensor() {
  tempSensor = new TempSensor(TEMP_SENSOR_PIN, lcd, BUZZER_PIN);
  tempSensor->setTZAddress(TZAddress);
  tempSensor->setTKWAddress(TKWAddress);
  tempSensor->setTCOAddress(TCOAddress);
  tempSensor->setTBAddress(TBAddress);
  tempSensor->setTCWUAddress(TCWUAddress);
  tempSensor->setTPAddress(TPAddress);
  tempSensor->diagnose(stopOnError);
}

void readTemperatures() {
  tz = tempSensor->readTZ();
  tkw = tempSensor->readTKW();
  tco = tempSensor->readTCO();
  tb = tempSensor->readTB();
  tcwu = tempSensor->readTCWU();
  tp = tempSensor->readTP();

  bool working = tempSensor->isTZInRange(tz) && tempSensor->isTKWInRange(tkw) &&
                 tempSensor->isTCOInRange(tco) && tempSensor->isTBInRange(tb) &&
                 tempSensor->isTCWUInRange(tcwu) && tempSensor->isTPInRange(tp);

  if (!working) {
    tempSensor->diagnose(stopOnError);
  }
}

void computeTKG() {
  const double KG = 1.0; // nachylenie krzywej grzania
  const double TW = 22;  // pożądana temperatura wewnątrz
  tkg = KG * (TW - tz) + TW;
}

void openZTCEnd() { digitalWrite(ZTC_PIN, LOW); }
void openZTZEnd() { digitalWrite(ZTZ_PIN, LOW); }

void controlZT() {
  if (shouldOpenZT()) {
    digitalWrite(ZTC_PIN, HIGH);
    digitalWrite(ZTZ_PIN, LOW);
    timer.setTimeout(openZTCEnd, 3100); // 1 st.
    timer.setTimeout(controlZT, 23100); // 20s przerwy + 3.1s na otwarcie
  } else if (shouldCloseZT()) {
    digitalWrite(ZTZ_PIN, HIGH);
    digitalWrite(ZTC_PIN, LOW);
    timer.setTimeout(openZTZEnd, 5100); // 5/3 st.
    timer.setTimeout(controlZT, 12100); // 7s przerwy + 5.1s na zamknięcie
  } else {
    timer.setTimeout(controlZT, 5000);
  }
}

void stopOnError() {
  timer.deleteAllTimers();

  digitalWrite(PKW_PIN, HIGH);
  digitalWrite(PCWU_PIN, LOW);
  digitalWrite(PCO_PIN, LOW);
  digitalWrite(ZTZ_PIN, HIGH);
  digitalWrite(ZTC_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
}

void updateDisplay() {
  lcd->clear();
  printTSKW();
  printTKW();
  printTCWU();
  printTCO();
  printTB();
  printTZ();
  printTKG();
  printTP();
}

void printTSKW() {
  lcd->setCursor(0, 0);
  lcd->print("Tskw ");
  lcd->print((int)tskw);
}

char buff[10];
void formatFloat(const float temp) {
  sprintf(buff, "%d.%d", abs((int)temp), abs((int)(temp * 10) % 10));
}

void printTKW() {
  formatFloat(tkw);
  lcd->setCursor(10, 0);
  lcd->print("Tkw ");
  lcd->print(buff);
}

void printTCWU() {
  formatFloat(tcwu);
  lcd->setCursor(0, 1);
  lcd->print("Tcwu ");
  lcd->print(buff);
}

void printTCO() {
  formatFloat(tco);
  lcd->setCursor(10, 1);
  lcd->print("Tco ");
  lcd->print(buff);
}

void printTB() {
  formatFloat(tb);
  lcd->setCursor(0, 2);
  lcd->print("Tb   ");
  lcd->print(buff);
}

void printTZ() {
  formatFloat(tz);
  lcd->setCursor(10, 2);
  lcd->print("Tz ");
  if (tz < 0) {
    lcd->print("-");
  } else {
    lcd->print(" ");
  }
  lcd->print(buff);
}

void printTKG() {
  formatFloat(tkg);
  lcd->setCursor(0, 3);
  lcd->print("Tkg  ");
  lcd->print(buff);
}

void printTP() {
  formatFloat(tp);
  lcd->setCursor(10, 3);
  lcd->print("Tp  ");
  lcd->print(buff);
}
