#include "DateTime.h"

#define SEC_1970_TO_2000 946684800

static const uint8_t dim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

DateTime::DateTime() {
  this->year = 2000;
  this->month = 1;
  this->day = 1;
  this->hour = 12;
  this->minute = 0;
  this->second = 0;
  this->dayOfWeek = 7;
}

long DateTime::toUnixTime() {
  uint16_t dc = day;
  for (uint8_t i = 0; i < (month - 1); i++)
    dc += dim[i];
  if ((month > 2) && (((year - 2000) % 4) == 0))
    ++dc;
  dc = dc + (365 * (year - 2000)) + (((year - 2000) + 3) / 4) - 1;

  return ((((((dc * 24L) + hour) * 60) + minute) * 60) + second) +
         SEC_1970_TO_2000;
}
