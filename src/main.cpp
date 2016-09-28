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

void requestTemperatures();
void requestAndReadTemperatures();
void readTemperatures();
void computeTKG();
void controlZT();
void updateDisplay();

void stopOnError();

int main() {
  setup();

  while (true) {
    loop();
  }

  return 0;
}

void setup() {
  initConfig();
  initPins();
  initLcd();
  initTempSensor();
  initTimer();

  computeTKG();
  controlZT(); // recursive function
  updateDisplay();
}

void loop() {
  if (shouldTurnOnPKW(&conf)) {
    digitalWrite(PKW_PIN, HIGH);
  } else if (shouldTurnOffPKW(&conf)) {
    digitalWrite(PKW_PIN, LOW);
  }

  if (shouldTurnOnPCWU(&conf)) {
    digitalWrite(PCWU_PIN, HIGH);
  } else if (shouldTurnOffPCWU(&conf)) {
    digitalWrite(PCWU_PIN, LOW);
  }

  if (shouldTurnOnPCO(&conf)) {
    digitalWrite(PCO_PIN, HIGH);
  } else if (shouldTurnOffPCO(&conf)) {
    digitalWrite(PCO_PIN, LOW);
  }

  timer.update();
}

void initConfig() {
  conf.tskw = 500;
  conf.ztcwu = 66;
  conf.kg = 0.9;
  conf.tw = 20.5;
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

  const int MEASUREMENT_TIME = 800;
  tempSensor->requestTemperatures();
  delay(MEASUREMENT_TIME);
  readTemperatures();
}

void initTimer() {
  timer.setInterval(requestTemperatures, 1000); // min. 800ms
  timer.setInterval(computeTKG, 600000);        // 10min.
  timer.setInterval(updateDisplay, 1000);
}

void requestTemperatures() {
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

  bool working =
      tempSensor->isTZInRange(conf.tz) && tempSensor->isTKWInRange(conf.tkw) &&
      tempSensor->isTCOInRange(conf.tco) && tempSensor->isTBInRange(conf.tb) &&
      tempSensor->isTCWUInRange(conf.tcwu) && tempSensor->isTPInRange(conf.tp);

  if (!working) {
    tempSensor->diagnose(stopOnError);
  }
}

void computeTKG() {
  // kg - krzywa grzewcza, tw - temperatura wewnątrz
  conf.tkg = conf.kg * (conf.tw - conf.tz) + conf.tw;
}

void openZTCEnd() { digitalWrite(ZTC_PIN, LOW); }
void openZTZEnd() { digitalWrite(ZTZ_PIN, LOW); }

void controlZT() {
  if (shouldOpenZT(&conf)) {
    digitalWrite(ZTC_PIN, HIGH);
    digitalWrite(ZTZ_PIN, LOW);
    timer.setTimeout(openZTCEnd, 3100); // 1 st.
    timer.setTimeout(controlZT, 23100); // 20s przerwy + 3.1s na otwarcie
  } else if (shouldCloseZT(&conf)) {
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
          total(conf.tskw), total(conf.tkw), fraction(conf.tkw),
          total(conf.tcwu), fraction(conf.tcwu), total(conf.tco),
          fraction(conf.tco), total(conf.tb), fraction(conf.tb),
          ((conf.tz < 0) ? '-' : ' '), total(conf.tz), fraction(conf.tz),
          total(conf.tkg), fraction(conf.tkg), total(conf.tp),
          fraction(conf.tp));

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
