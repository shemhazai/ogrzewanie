#ifndef CONFIG_H
#define CONFIG_H

class Config {
public:
  float tz;    // Temp. zewnętrzna
  float tkw;   // Temp. kotła węglowego
  float tskw;  // Temp. spalin kotła węglowego
  float tco;   // Temp. centralnego ogrzewania
  float tb;    // Temp. bufora
  float tcwu;  // Temp. ciepłej wody użytkowej
  float tkg;   // Temp. krzywa grzewcza
  float tp;    // Temp. powrotu
  float ztcwu; // Zadana temp. ciepłej wody użytkowej
  float kg;    // Krzywa grzewcza
  float tw;    // Pożadana temp. wewnątrz
  float zth;   // Zawór trójdrogowy, histereza
  float tcwuh; // Histereza ciepłej wody użytkowej
  float minrb; // Min. rezerwa bufora, o ile temp. bufora musi być wyższa od
               // temp. ciepłej wody użytkowej aby nie kończyć grzać cwu.
  float maxrb; // Max. rezerwa bufora, o ile temp. bufora musi być wyższa od
               // temp. ciepłej wody użytkowej, żeby zacząć grzać cwu
  float ztos;  // Zawór trójdrogowy, o ile otworzyć (w stopniach)
  float ztzs;  // Zawór trójdrogowy, o ile zamknąć (w stopniach)

  bool shouldTurnOnPKW();
  bool shouldTurnOffPKW();
  bool shouldTurnOnPCWU();
  bool shouldTurnOffPCWU();
  bool shouldTurnOnPCO();
  bool shouldTurnOffPCO();
  bool shouldOpenZT();
  bool shouldCloseZT();
};

#endif
