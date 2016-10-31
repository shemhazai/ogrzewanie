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
  ztb = 80;
  ipa = 1;

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
  tb = memory->readZTB();
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
  memory->writeZTB(ztb);
  memory->writeIPA(ipa);
  memory->setConfigSaved(true);
}

bool Config::shouldTurnOnPKW() {
  return ((tkw >= 46) && (tskw >= 110)) || tkw >= 95;
}

bool Config::shouldTurnOffPKW() { return tskw <= 100 && tkw <= 93; }

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

void Config::setProperty(String name, float value) {
  name.toLowerCase();
  name.trim();

  if (name.equals("ztb")) {
    memory->writeZTB(value);
    ztb = value;
  } else if (name.equals("ztcwu")) {
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
