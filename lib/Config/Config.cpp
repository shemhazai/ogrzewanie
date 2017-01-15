#include "Config.h"

Config::Config(Memory *aMemory) {
  memory = aMemory;

  tz = tw = tkw = tskw = tco = tb = tcwu = tkg = tp = tko = tsko = 0;
  pkw = pko = pco = pcwu = false;
  ztcwu = 60;
  kg = 0.9;
  ptw = 21;
  zth = 1.1;
  tcwuh = 1.0;
  minrb = 2.0;
  maxrb = 5.0;
  ztos = 1.0;
  ztzs = 1.6;
  ipa = 1;
  kor = 0;
  ko = false;

  if (memory->isConfigSaved()) {
    readConfig();
  } else {
    saveConfig();
  }
}

void Config::readConfig() {
  ztcwu = memory->readZTCWU();
  kg = memory->readKG();
  ptw = memory->readPTW();
  zth = memory->readZTH();
  tcwuh = memory->readTCWUH();
  minrb = memory->readMINRB();
  maxrb = memory->readMAXRB();
  ztos = memory->readZTOS();
  ztzs = memory->readZTZS();
  ipa = memory->readIPA();
}

void Config::saveConfig() {
  memory->writeZTCWU(ztcwu);
  memory->writeKG(kg);
  memory->writePTW(ptw);
  memory->writeZTH(zth);
  memory->writeTCWUH(tcwuh);
  memory->writeMINRB(minrb);
  memory->writeMAXRB(maxrb);
  memory->writeZTOS(ztos);
  memory->writeZTZS(ztzs);
  memory->writeIPA(ipa);
  memory->setConfigSaved(true);
}

bool Config::shouldTurnOnPKW() {
  return ((tkw >= 46) && (tskw >= 180)) || tkw >= 93;
}

bool Config::shouldTurnOffPKW() { return tskw <= 150 && tkw <= 91; }

bool Config::shouldTurnOnPCWU() {
  return (tb >= (tcwu + maxrb)) && (tcwu <= (ztcwu - tcwuh));
}

bool Config::shouldTurnOffPCWU() {
  return (tb <= (tcwu + minrb)) || (tcwu >= (ztcwu + tcwuh));
}

bool Config::shouldTurnOnPCO() { return tz < 17; }

bool Config::shouldTurnOffPCO() { return tz >= 18; }

bool Config::shouldOpenZT() { return (tco <= (tkg - zth)); }

bool Config::shouldCloseZT() { return (tco >= (tkg + zth)); }

bool Config::shouldBeep() { return (tskw > 470) || (tkw > 95) || (tb > 95); }

bool Config::shouldTurnOnKO() {
  return ko && (tb < 68.0) && (tko < 75.0) && (tskw < 80.0);
}

bool Config::shouldTurnOffKO() {
  return (tb > 80.0) || (tko > 81.0);
}

bool Config::shouldTurnOnPKO() {
  return ((tsko > 150.0) && (tko > 45.0) && (tb <= 80.0))
    || (tko - tb > 2);
}

bool Config::shouldTurnOffPKO() {
  return (tko <= tb);
}

bool Config::isKODisabled() {
  return !ko;
}

bool Config::shouldTurnOnPFA() {
  return tsko < 150;
}

void Config::setProperty(String name, float value) {
  name.toLowerCase();
  name.trim();

  if (name.equals("ztcwu")) {
    memory->writeZTCWU(value);
    ztcwu = value;
  } else if (name.equals("kg")) {
    memory->writeKG(value);
    kg = value;
  } else if (name.equals("ptw")) {
    memory->writePTW(value);
    ptw = value;
  } else if (name.equals("zth")) {
    memory->writeZTH(value);
    zth = value;
  } else if (name.equals("tcwuh")) {
    memory->writeTCWUH(value);
    tcwuh = value;
  } else if (name.equals("minrb")) {
    memory->writeMINRB(value);
    minrb = value;
  } else if (name.equals("maxrb")) {
    memory->writeMAXRB(value);
    maxrb = value;
  } else if (name.equals("ztos")) {
    memory->writeZTOS(value);
    ztos = value;
  } else if (name.equals("ztzs")) {
    memory->writeZTZS(value);
    ztzs = value;
  } else if (name.equals("ipa")) {
    memory->writeIPA((int)value);
    ipa = (int)value;
  }
}

void Config::computeTKG() {
  float t = (ptw - 20.0) * 4.2;
  float kg3 = kg * kg * kg;
  float corr = 0;

  float delta = ptw - tw;
  if (delta >= 0.2) {
    corr = 5.0;
    kor = 1;
  } else if (delta <= -0.2) {
    corr = -5.0;
    kor = -1;
  } else if ((kor == 1) && (tw < ptw)) {
    corr = 5.0;
  } else {
    kor = 0;
  }

  tkg = -0.75 * kg3 * (tz - t) + (44 * kg) + t + corr;
}
