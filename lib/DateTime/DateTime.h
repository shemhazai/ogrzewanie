#ifndef DATETIME_H
#define DATETIME_H

#include <Arduino.h>

class DateTime {
public:
  DateTime();
  long toUnixTime();

  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t dayOfWeek;
};

#endif
