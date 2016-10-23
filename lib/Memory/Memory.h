#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <EEPROM.h>

class Memory {
public:
  bool isConfigSaved();

  float readZTCWU();
  float readKG();
  float readTW();
  float readZTH();
  float readTCWUH();
  float readMINRB();
  float readMAXRB();
  float readZTOS();
  float readZTZS();
  float readZTB();

  void setConfigSaved(bool saved);

  void writeZTCWU(const float ztcwu);
  void writeKG(const float kg);
  void writeTW(const float tw);
  void writeZTH(const float zth);
  void writeTCWUH(const float tcwuh);
  void writeMINRB(const float minrb);
  void writeMAXRB(const float maxrb);
  void writeZTOS(const float ztos);
  void writeZTZS(const float ztzs);
  void writeZTB(const float ztb);

private:
  void writeFloat(int address, const float val);
  void writeInt(int address, const int val);

  float readFloat(int address);
  int readInt(int address);

  const static int CONFIG_SAVED = 255;
  const static int CONFIG_NOT_SAVED = 0;

  const static int CONFIG_SAVED_ADDRESS = 0;
  const static int ZTCWU_ADDRESS = 2;
  const static int KG_ADDRESS = 6;
  const static int TW_ADDRESS = 10;
  const static int ZTH_ADDRESS = 14;
  const static int TCWUH_ADDRESS = 18;
  const static int MINRB_ADDRESS = 22;
  const static int MAXRB_ADDRESS = 26;
  const static int ZTOS_ADDRESS = 30;
  const static int ZTZS_ADDRESS = 34;
  const static int ZTB_ADDRESS = 38;
};

#endif
