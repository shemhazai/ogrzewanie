#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TempSensor.h>
#include <Timer.h>

LiquidCrystal *lcd;
TempSensor *tempSensor;
Timer timer;

float tz, tkw, tskw = 500, tco, tb, tcwu, tkg, tp;
float tcwug = 64;

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

inline bool shouldTurnOnPKW() { return tkw >= 40 && tskw >= 100; }
inline bool shouldTurnOffPKW() { return tskw <= 100; }
inline bool shouldTurnOnPCWU() {
  const float HISTERESIS = 1;
  const float BUFFER_RESERVE = 5;
  return (tb >= (tcwu + BUFFER_RESERVE)) && (tcwu < (tcwug - HISTERESIS));
}
inline bool shouldTurnOffPCWU() {
  const float HISTERESIS = 1;
  const float BUFFER_RESERVE = 2;
  return (tb <= tcwu + BUFFER_RESERVE) || (tcwu >= (tcwug + HISTERESIS));
}
inline bool shouldTurnOnPCO() { return tz < 17; }
inline bool shouldTurnOffPCO() { return tz >= 18; }
inline bool shouldOpenZT() {
  const float HISTERESIS = 1.5;
  return tco <= tkg - HISTERESIS;
}
inline bool shouldCloseZT() {
  const float HISTERESIS = 1.5;
  return tco >= tkg + HISTERESIS;
}

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
  const double KG = 1.0;  // nachylenie krzywej grzania
  const double TW = 20.5; // pożądana temperatura wewnątrz
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

inline int total(float temp) {
  int total = (int)temp;
  return abs(total);
}

inline int fraction(float temp) {
  int fraction = (int)(temp * 10) % 10;
  return abs(fraction);
}

void updateDisplay() {
  char text[84];
  sprintf(text, "Tskw %3d  Tkw %2d.%d\n"
                "Tcwu %2d.%d Tco %2d.%d\n"
                "Tb   %2d.%d Tz %c%2d.%d\n"
                "Tkg  %2d.%d Tp  %2d.%d",
          total(tskw), total(tkw), fraction(tkw), total(tcwu), fraction(tcwu),
          total(tco), fraction(tco), total(tb), fraction(tb),
          ((tz < 0) ? '-' : ' '), total(tz), fraction(tz), total(tkg),
          fraction(tkg), total(tp), fraction(tp));

  lcd->clear();
  lcd->setCursor(0, 0);

  const uint8_t len = strlen(text);
  uint8_t line = 0;
  for (uint8_t i = 0; i < len; i++) {
    if (text[i] == '\n') {
      line++;
      lcd->setCursor(0, line);
    } else {
      lcd->print(text[i]);
    }
  }
}
