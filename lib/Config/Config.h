#ifndef CONFIG_H
#define CONFIG_H

#include <Memory.h>

class Config {
public:
  Config(Memory *aMemory);

  bool shouldTurnOnPKW();
  bool shouldTurnOffPKW();
  bool shouldTurnOnPCWU();
  bool shouldTurnOffPCWU();
  bool shouldTurnOnPCO();
  bool shouldTurnOffPCO();
  bool shouldOpenZT();
  bool shouldCloseZT();
  bool shouldBeep();

  void setProperty(String name, float value);

  float tz;    // Temp. zewnętrzna
  float tw;    // Temp. wewnętrzna
  float tkw;   // Temp. kotła węglowego
  float tskw;  // Temp. spalin kotła węglowego
  float tco;   // Temp. centralnego ogrzewania
  float ztko;  // Zadana temp. kotła olejowego
  float tb;    // Temp. bufora
  float tcwu;  // Temp. ciepłej wody użytkowej
  float tkg;   // Temp. krzywa grzewcza
  float tp;    // Temp. powrotu
  float ztcwu; // Zadana temp. ciepłej wody użytkowej
  float kg;    // Krzywa grzewcza
  float ptw;   // Pożadana temp. wewnątrz
  float zth;   // Zawór trójdrogowy, histereza
  float tcwuh; // Histereza ciepłej wody użytkowej
  float minrb; // Min. rezerwa bufora, o ile temp. bufora musi być wyższa od
               // temp. ciepłej wody użytkowej aby nie kończyć grzać cwu.
  float maxrb; // Max. rezerwa bufora, o ile temp. bufora musi być wyższa od
               // temp. ciepłej wody użytkowej, żeby zacząć grzać cwu
  float ztos;  // Zawór trójdrogowy, o ile otworzyć (w stopniach)
  float ztzs;  // Zawór trójdrogowy, o ile zamknąć (w stopniach)
  int ipa;     // Ilość prób odczytu temperatury do awarii
  bool pkw;    // Pompa kotła węglowego
  bool pko;    // Pompa kotła olejowego
  bool pco;    // Pompa centralnego ogrzewania
  bool pcwu;   // Pompa ciepłej wody użytkowej
  float tko;   // Temp. kotła olejowego
  float tsko;  // Temp. spalin kotła olejowego

private:
  void saveConfig();
  void readConfig();

  Memory *memory;
};

#endif
