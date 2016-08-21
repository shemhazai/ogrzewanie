#include "config.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TempSensor.h>

LiquidCrystal lcd(RS, ENABLE, D4, D5, D6, D7);
TempSensor tempSensor(TEMP_SENSOR_PIN, &lcd, BUZZER_PIN);

int main() {
  while (true)
    continue;
  return 0;
}
