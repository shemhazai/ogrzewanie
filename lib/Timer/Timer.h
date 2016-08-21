#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer {
public:
  const static uint8_t MAX_TIMERS = 10;

  Timer();

  int8_t setTimeout(void (*function)(), unsigned long timeout);
  int8_t setInterval(void (*function)(), unsigned long interval);
  void enableTimer(uint8_t slot);
  void disableTimer(uint8_t slot);
  void deleteTimer(uint8_t slot);
  void update();

private:
  int8_t findTimerSlot();

  void (*functions[MAX_TIMERS])();
  unsigned long intervals[MAX_TIMERS];
  unsigned long currentIntervals[MAX_TIMERS];
  bool repeat[MAX_TIMERS];
  bool enabled[MAX_TIMERS];

  unsigned long lastUpdate;
};

#endif
