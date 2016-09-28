#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define TEMP_SENSOR_PIN A2
#define BUZZER_PIN 44
#define PKW_PIN 51
#define PCWU_PIN 52
#define PCO_PIN 53
#define ZTC_PIN 49
#define ZTZ_PIN 48

#define MAX_SCK 33
#define MAX_CS 34
#define MAX_SO 35

#define RS 13
#define RW 12
#define ENABLE 11
#define D0 10
#define D1 9
#define D2 8
#define D3 7
#define D4 6
#define D5 5
#define D6 4
#define D7 3

const uint8_t TZAddress[] = {0x28, 0xFF, 0x11, 0xB1, 0x64, 0x14, 0x02, 0xB7};
const uint8_t TKWAddress[] = {0x28, 0xFF, 0x27, 0x53, 0x6F, 0x14, 0x04, 0xFD};
const uint8_t TCOAddress[] = {0x28, 0xFF, 0x49, 0x35, 0x64, 0x14, 0x01, 0x29};
const uint8_t TBAddress[] = {0x28, 0xB9, 0xE9, 0xC6, 0x07, 0x00, 0x00, 0xEC};
const uint8_t TCWUAddress[] = {0x28, 0x06, 0xC0, 0x04, 0x07, 0x00, 0x00, 0xFA};
const uint8_t TPAddress[] = {0x28, 0x38, 0xE7, 0xC6, 0x07, 0x00, 0x00, 0x93};

struct Config {
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
};
#endif
