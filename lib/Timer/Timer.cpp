#include "Timer.h"

Timer::Timer() {
  for (int i = 0; i < MAX_TIMERS; i++)
    functions[i] = 0;
  lastUpdate = millis();
}

int8_t Timer::setTimeout(void (*function)(), unsigned long timeout) {
  int8_t slot = findTimerSlot();
  if (slot == -1)
    return slot;

  functions[slot] = function;
  intervals[slot] = timeout;
  currentIntervals[slot] = timeout;
  repeat[slot] = false;
  enabled[slot] = true;
  return slot;
}

int8_t Timer::setInterval(void (*function)(), unsigned long interval) {
  int8_t slot = findTimerSlot();
  if (slot == -1)
    return slot;

  functions[slot] = function;
  intervals[slot] = interval;
  currentIntervals[slot] = interval;
  repeat[slot] = true;
  enabled[slot] = true;
  return slot;
}

void Timer::enableTimer(uint8_t slot) {
  if (slot >= 0 && slot <= MAX_TIMERS)
    enabled[slot] = true;
}

void Timer::disableTimer(uint8_t slot) {
  if (slot >= 0 && slot <= MAX_TIMERS)
    enabled[slot] = false;
}

void Timer::deleteTimer(uint8_t slot) {
  if (slot >= 0 && slot <= MAX_TIMERS)
    functions[slot] = 0;
}

void Timer::update() {
  unsigned int updateTime = millis() - lastUpdate;
  lastUpdate = millis();

  for (int i = 0; i < MAX_TIMERS; i++) {
    if ((functions[i] == 0) || !enabled[i])
      continue;

    if (currentIntervals[i] <= updateTime) {
      functions[i]();
      if (repeat[i]) {
        currentIntervals[i] = intervals[i];
      } else {
        functions[i] = 0;
      }
    } else {
      currentIntervals[i] -= updateTime;
    }
  }
}

int8_t Timer::findTimerSlot() {
  for (int i = 0; i < MAX_TIMERS; i++) {
    if (functions[i] == 0)
      return i;
  }
  return -1;
}
