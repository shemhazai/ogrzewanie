#include "condition.h"
#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TempSensor.h>
#include <Timer.h>

struct Config conf;
LiquidCrystal *lcd;
TempSensor *tempSensor;
Timer timer;

void setup();
void loop();

void initConfig();
void initPins();
void initLcd();
void initTempSensor();
void initTimer();

void requestAndReadTemperatures();
void readTemperatures();
void computeTKG();
void controlZT();
void updateDisplay();
void printText(const char *text);

void stopOnError();

int main() {
  setup();
  while (true)
    loop();
  return 0;
}

void setup() {
  initConfig();
  initPins();
  initLcd();
  initTempSensor();
  initTimer();

  // beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);

  computeTKG();
  controlZT(); // recursive function
  updateDisplay();
}

void loop() {
  if (shouldTurnOnPKW(conf)) {
    digitalWrite(PKW_PIN, HIGH);
  } else if (shouldTurnOffPKW(conf)) {
    digitalWrite(PKW_PIN, LOW);
  }

  if (shouldTurnOnPCWU(conf)) {
    digitalWrite(PCWU_PIN, HIGH);
  } else if (shouldTurnOffPCWU(conf)) {
    digitalWrite(PCWU_PIN, LOW);
  }

  if (shouldTurnOnPCO(conf)) {
    digitalWrite(PCO_PIN, HIGH);
  } else if (shouldTurnOffPCO(conf)) {
    digitalWrite(PCO_PIN, LOW);
  }

  timer.update();
}

void initConfig() {
  conf.ztcwu = 66;
  conf.kg = 0.9;
  conf.tw = 20.5;
  conf.zth = 1.5;
  conf.tcwuh = 1.0;
  conf.minrb = 2.0;
  conf.maxrb = 5.0;
  conf.ztos = 1.0;
  conf.ztzs = 1.6;
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
  tempSensor = new TempSensor(TEMP_SENSOR_PIN);
  tempSensor->setMAX6675Pins(MAX_SCK, MAX_CS, MAX_SO);
  tempSensor->setTZAddress(TZAddress);
  tempSensor->setTKWAddress(TKWAddress);
  tempSensor->setTCOAddress(TCOAddress);
  tempSensor->setTBAddress(TBAddress);
  tempSensor->setTCWUAddress(TCWUAddress);
  tempSensor->setTPAddress(TPAddress);

  const int MEASUREMENT_TIME = 800;
  tempSensor->requestTemperatures();
  delay(MEASUREMENT_TIME);
  readTemperatures();
}

void initTimer() {
  timer.setInterval(requestAndReadTemperatures, 1000); // min. 800ms
  timer.setInterval(computeTKG, 600000);               // 10min.
  timer.setInterval(updateDisplay, 1000);
}

void requestAndReadTemperatures() {
  const int MEASUREMENT_TIME = 800;
  tempSensor->requestTemperatures();
  timer.setTimeout(readTemperatures, MEASUREMENT_TIME);
}

void readTemperatures() {
  conf.tz = tempSensor->readTZ();
  conf.tkw = tempSensor->readTKW();
  conf.tco = tempSensor->readTCO();
  conf.tb = tempSensor->readTB();
  conf.tcwu = tempSensor->readTCWU();
  conf.tp = tempSensor->readTP();
  conf.tskw = tempSensor->readTSKW();

  bool isTZWorking = tempSensor->isTZInRange(conf.tz);
  bool isTKWWorking = tempSensor->isTKWInRange(conf.tkw);
  bool isTCOWorking = tempSensor->isTCOInRange(conf.tco);
  bool isTBWorking = tempSensor->isTBInRange(conf.tb);
  bool isTCWUWorking = tempSensor->isTCWUInRange(conf.tcwu);
  bool isTPWorking = tempSensor->isTPInRange(conf.tp);
  bool isTSKWWorking = tempSensor->isTSKWInRange(conf.tskw);

  bool working = isTZWorking && isTKWWorking && isTCOWorking && isTBWorking &&
                 isTCWUWorking && isTPWorking && isTSKWWorking;

  if (!working) {
    stopOnError();

    char text[84];
    sprintf(text, "Tcwu: %c  Tz: %c\n"
                  "Tkw : %c  Tb: %c\n"
                  "Tco : %c  Tp: %c\n"
                  "Tskw: %c",
            (isTCWUWorking ? '+' : '-'), (isTZWorking ? '+' : '-'),
            (isTKWWorking ? '+' : '-'), (isTBWorking ? '+' : '-'),
            (isTCOWorking ? '+' : '-'), (isTPWorking ? '+' : '-'),
            (isTSKWWorking ? '+' : '-'));
    printText(text);
    while (true)
      delay(1000);
  }
}

void computeTKG() {
  // kg - krzywa grzewcza, tw - temperatura wewnątrz
  conf.tkg = conf.kg * (conf.tw - conf.tz) + conf.tw;
}

void openZTCEnd() { digitalWrite(ZTC_PIN, LOW); }
void openZTZEnd() { digitalWrite(ZTZ_PIN, LOW); }

void controlZT() {
  if (shouldOpenZT(conf)) {
    digitalWrite(ZTC_PIN, HIGH);
    digitalWrite(ZTZ_PIN, LOW);

    const int openTime = (int)(conf.ztos * 3.111); // 280s / 90st.
    timer.setTimeout(openZTCEnd, openTime);
    timer.setTimeout(controlZT, openTime + 20000);
  } else if (shouldCloseZT(conf)) {
    digitalWrite(ZTZ_PIN, HIGH);
    digitalWrite(ZTC_PIN, LOW);

    const int closeTime = (int)(conf.ztzs * 3.111); // 280s / 90st.
    timer.setTimeout(openZTZEnd, closeTime);
    timer.setTimeout(controlZT, closeTime + 7000);
  } else {
    timer.setTimeout(controlZT, 5000);
  }
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
  sprintf(text, "Tskw %3d  Tkw %2d.%1d\n"
                "Tcwu %2d.%1d Tco %2d.%1d\n"
                "Tb   %2d.%1d Tz %c%2d.%1d\n"
                "Tkg  %2d.%1d Tp  %2d.%1d",
          total(conf.tskw), total(conf.tkw), fraction(conf.tkw),
          total(conf.tcwu), fraction(conf.tcwu), total(conf.tco),
          fraction(conf.tco), total(conf.tb), fraction(conf.tb),
          ((conf.tz < 0) ? '-' : ' '), total(conf.tz), fraction(conf.tz),
          total(conf.tkg), fraction(conf.tkg), total(conf.tp),
          fraction(conf.tp));

  printText(text);
}

void printText(const char *text) {
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

void stopOnError() {
  timer.deleteAllTimers();

  digitalWrite(PKW_PIN, HIGH);
  digitalWrite(PCWU_PIN, LOW);
  digitalWrite(PCO_PIN, LOW);
  digitalWrite(ZTZ_PIN, HIGH);
  digitalWrite(ZTC_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
}
