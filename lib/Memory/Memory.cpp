#include "Memory.h"

bool Memory::isConfigSaved() {
  return readInt(CONFIG_SAVED_ADDRESS) == CONFIG_SAVED;
}

void Memory::setConfigSaved(bool saved) {
  if (saved) {
    writeInt(CONFIG_SAVED_ADDRESS, CONFIG_SAVED);
  } else {
    writeInt(CONFIG_SAVED_ADDRESS, CONFIG_NOT_SAVED);
  }
}

float Memory::readZTCWU() { return readFloat(ZTCWU_ADDRESS); }

float Memory::readKG() { return readFloat(KG_ADDRESS); }

float Memory::readPTW() { return readFloat(PTW_ADDRESS); }

float Memory::readZTH() { return readFloat(ZTH_ADDRESS); }

float Memory::readTCWUH() { return readFloat(TCWUH_ADDRESS); }

float Memory::readMINRB() { return readFloat(MINRB_ADDRESS); }

float Memory::readMAXRB() { return readFloat(MAXRB_ADDRESS); }

float Memory::readZTOS() { return readFloat(ZTOS_ADDRESS); }

float Memory::readZTZS() { return readFloat(ZTZS_ADDRESS); }

float Memory::readZTB() { return readFloat(ZTB_ADDRESS); }

int Memory::readIPA() { return readInt(IPA_ADDRESS); }

void Memory::writeZTCWU(const float ztcwu) { writeFloat(ZTCWU_ADDRESS, ztcwu); }

void Memory::writeKG(const float kg) { writeFloat(KG_ADDRESS, kg); }

void Memory::writePTW(const float ptw) { writeFloat(PTW_ADDRESS, ptw); }

void Memory::writeZTH(const float zth) { writeFloat(ZTH_ADDRESS, zth); }

void Memory::writeTCWUH(const float tcwuh) { writeFloat(TCWUH_ADDRESS, tcwuh); }

void Memory::writeMINRB(const float minrb) { writeFloat(MINRB_ADDRESS, minrb); }

void Memory::writeMAXRB(const float maxrb) { writeFloat(MAXRB_ADDRESS, maxrb); }

void Memory::writeZTOS(const float ztos) { writeFloat(ZTOS_ADDRESS, ztos); }

void Memory::writeZTZS(const float ztzs) { writeFloat(ZTZS_ADDRESS, ztzs); }

void Memory::writeZTB(const float ztb) { writeFloat(ZTB_ADDRESS, ztb); }

void Memory::writeIPA(const int ipa) { writeInt(IPA_ADDRESS, ipa); }

void Memory::writeFloat(int address, const float val) {
  const uint8_t *p = (const uint8_t *)(const void *)&val;
  for (uint8_t i = 0; i < sizeof(float); i++)
    EEPROM.write(address++, *p++);
}

void Memory::writeInt(int address, const int val) {
  const uint8_t *p = (const uint8_t *)(const void *)&val;
  for (uint8_t i = 0; i < sizeof(int); i++)
    EEPROM.write(address++, *p++);
}

float Memory::readFloat(int address) {
  float value;
  uint8_t *p = (uint8_t *)(void *)&value;
  for (uint8_t i = 0; i < sizeof(float); i++)
    *p++ = EEPROM.read(address++);
  return value;
}

int Memory::readInt(int address) {
  int value;
  uint8_t *p = (uint8_t *)(void *)&value;
  for (uint8_t i = 0; i < sizeof(int); i++)
    *p++ = EEPROM.read(address++);
  return value;
}
