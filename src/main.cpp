#include "pins.h"
#include <Arduino.h>
#include <Config.h>
#include <LiquidCrystal.h>
#include <Memory.h>
#include <TempSensor.h>
#include <Timer.h>
#include <WebServer.h>

#include <EEPROM.h>
#include <Ethernet.h>
#include <SPI.h>

Memory memory;
Config *conf;
WebServer *server;
LiquidCrystal *lcd;
TempSensor *tempSensor;
Timer timer;

int tempReadErrorCount = 0;

void setup();
void loop();

void initConfig();
void initPins();
void initLcd();
void initTempSensor();
void initTimer();
void initServer();

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
  initServer();

  // beep
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);

  computeTKG();
  controlZT(); // recursive function
  updateDisplay();
}

void loop() {
  if (conf->shouldTurnOnPKW()) {
    digitalWrite(PKW_PIN, HIGH);
    conf->pkw = true;
  } else if (conf->shouldTurnOffPKW()) {
    digitalWrite(PKW_PIN, LOW);
    conf->pkw = false;
  }

  if (conf->shouldTurnOnPCWU()) {
    digitalWrite(PCWU_PIN, HIGH);
    conf->pcwu = true;
  } else if (conf->shouldTurnOffPCWU()) {
    digitalWrite(PCWU_PIN, LOW);
    conf->pcwu = false;
  }

  if (conf->shouldTurnOnPCO()) {
    digitalWrite(PCO_PIN, HIGH);
    conf->pco = true;
  } else if (conf->shouldTurnOffPCO()) {
    digitalWrite(PCO_PIN, LOW);
    conf->pco = false;
  }

  server->handleClient();
  timer.update();
}

void initConfig() { conf = new Config(&memory); }

void initPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PKW_PIN, OUTPUT);
  pinMode(PCWU_PIN, OUTPUT);
  pinMode(PCO_PIN, OUTPUT);
  pinMode(ZTZ_PIN, OUTPUT);
  pinMode(ZTC_PIN, OUTPUT);
  pinMode(ETHERNET_SHIELD_SPI_SWITCH, OUTPUT);
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
  tempSensor->setTWAddress(TWAddress);

  const int MEASUREMENT_TIME = 1800;
  tempSensor->requestTemperatures();
  delay(MEASUREMENT_TIME);
  readTemperatures();
}

void initTimer() {
  timer.update();                                      // set lastUpdate
  timer.setInterval(requestAndReadTemperatures, 2000); // min. 1800ms
  timer.setInterval(computeTKG, 600000);               // 10min.
  timer.setInterval(updateDisplay, 1000);
}

void initServer() { server = new WebServer(conf); }

void requestAndReadTemperatures() {
  const int MEASUREMENT_TIME = 1800;
  tempSensor->requestTemperatures();
  timer.setTimeout(readTemperatures, MEASUREMENT_TIME);
}

void readTemperatures() {
  float tz = tempSensor->readTZ();
  float tkw = tempSensor->readTKW();
  float tco = tempSensor->readTCO();
  float tb = tempSensor->readTB();
  float tcwu = tempSensor->readTCWU();
  float tp = tempSensor->readTP();
  float tskw = tempSensor->readTSKW();
  float tw = tempSensor->readTW();

  bool isTZWorking = tempSensor->isTZInRange(tz);
  bool isTKWWorking = tempSensor->isTKWInRange(tkw);
  bool isTCOWorking = tempSensor->isTCOInRange(tco);
  bool isTBWorking = tempSensor->isTBInRange(tb);
  bool isTCWUWorking = tempSensor->isTCWUInRange(tcwu);
  bool isTPWorking = tempSensor->isTPInRange(tp);
  bool isTSKWWorking = tempSensor->isTSKWInRange(tskw);
  bool isTWWorking = tempSensor->isTWInRange(tw);

  bool working = isTZWorking && isTKWWorking && isTCOWorking && isTBWorking &&
                 isTCWUWorking && isTPWorking && isTSKWWorking && isTWWorking;

  if (!working) {
    tempReadErrorCount++;

    if (tempReadErrorCount == conf->ipa) {
      stopOnError();

      char text[84];
      sprintf(text, "Tcwu: %c  Tz: %c\n"
                    "Tkw : %c  Tb: %c\n"
                    "Tco : %c  Tp: %c\n"
                    "Tskw: %c  Tw: %c",
              (isTCWUWorking ? '+' : '-'), (isTZWorking ? '+' : '-'),
              (isTKWWorking ? '+' : '-'), (isTBWorking ? '+' : '-'),
              (isTCOWorking ? '+' : '-'), (isTPWorking ? '+' : '-'),
              (isTSKWWorking ? '+' : '-'), (isTWWorking ? '+' : '-'));
      printText(text);
      while (true)
        delay(1000);
    }
  } else {
    tempReadErrorCount = 0;
  }

  conf->tz = isTZWorking ? tz : conf->tz;
  conf->tkw = isTKWWorking ? tkw : conf->tkw;
  conf->tco = isTCOWorking ? tco : conf->tco;
  conf->tb = isTBWorking ? tb : conf->tb;
  conf->tcwu = isTCWUWorking ? tcwu : conf->tcwu;
  conf->tskw = isTSKWWorking ? tskw : conf->tskw;
  conf->tp = isTPWorking ? tp : conf->tp;
  conf->tw = isTWWorking ? tw : conf->tw;
}

void computeTKG() {
  // kg - krzywa grzewcza, tw - temperatura wewnątrz
  conf->tkg = conf->kg * (conf->ptw - conf->tz) + conf->ptw;
}

void openZTCEnd() { digitalWrite(ZTC_PIN, LOW); }
void openZTZEnd() { digitalWrite(ZTZ_PIN, LOW); }

void controlZT() {
  if (conf->shouldOpenZT()) {
    digitalWrite(ZTC_PIN, HIGH);
    digitalWrite(ZTZ_PIN, LOW);

    const int openTime = (int)(conf->ztos * 3111); // 280s / 90st.
    timer.setTimeout(openZTCEnd, openTime);
    timer.setTimeout(controlZT, openTime + 20000);
  } else if (conf->shouldCloseZT()) {
    digitalWrite(ZTZ_PIN, HIGH);
    digitalWrite(ZTC_PIN, LOW);

    const int closeTime = (int)(conf->ztzs * 3111); // 280s / 90st.
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
          total(conf->tskw), total(conf->tkw), fraction(conf->tkw),
          total(conf->tcwu), fraction(conf->tcwu), total(conf->tco),
          fraction(conf->tco), total(conf->tb), fraction(conf->tb),
          ((conf->tz < 0) ? '-' : ' '), total(conf->tz), fraction(conf->tz),
          total(conf->tkg), fraction(conf->tkg), total(conf->tp),
          fraction(conf->tp));

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
