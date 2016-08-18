#include "temp_sensor.h"
#include <Arduino.h>
#include <DateTime.h>
#include <RealTimeClock.h>

void initSerial();
void printFormattedFloat();
void printRTCTime();

RealTimeClock rtc(SDA, SCL);

int main() {
  initSerial();
  // printFormattedFloat();

  rtc.begin();
  while (true) {
    printRTCTime();
    delay(1);
  }

  return 0;
}

void initSerial() {
  Serial.begin(9600);
  while (!Serial)
    continue;
}

void printFormattedFloat() {
  char text[50];
  sprintf(text, "temperature is %d.%d", (int)5.3, (int)(5.3 * 10) % 10);
  Serial.println(text);
  Serial.flush();
}

const char *dayOfWeekToString(int dayOfWeek) {
  const char *days[] = {"",         "Poniedzialek", "Wtorek", "Sroda",
                        "Czwartek", "Piatek",       "Sobota", "Niedziela"};
  return days[dayOfWeek];
}

void printRTCTime() {
  DateTime t = rtc.getDateTime();
  char text[100];
  sprintf(text, "%s %02d:%02d:%02d %02d/%02d/%d",
          dayOfWeekToString(t.dayOfWeek), t.hour, t.minute, t.second, t.day,
          t.month, t.year);
  Serial.println(text);
  Serial.flush();
}
