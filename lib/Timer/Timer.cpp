#include "Timer.h"

Timer::Timer() {
  for (uint8_t i = 0; i < MAX_TIMERS; i++)
    tasks[i] = NULL;
  lastUpdate = millis();
}

Timer::~Timer() { deleteAllTimers(); }

int8_t Timer::setTimeout(void (*function)(), unsigned long timeout) {
  if (timeout == 0)
    return -1;

  int8_t slot = findTimerSlot();
  if (slot == -1) {
    return slot;
  } else {
    struct TimerTask *task = new TimerTask();
    task->function = function;
    task->interval = 0;
    task->timeLeft = timeout;
    task->enabled = true;
    tasks[slot] = task;
    return slot;
  }
}

int8_t Timer::setInterval(void (*function)(), unsigned long interval) {
  if (interval == 0)
    return -1;

  int8_t slot = findTimerSlot();
  if (slot == -1) {
    return slot;
  } else {
    struct TimerTask *task = new TimerTask();
    task->function = function;
    task->interval = interval;
    task->timeLeft = interval;
    task->enabled = true;
    tasks[slot] = task;
    return slot;
  }
}

void Timer::enableTimer(uint8_t slot) {
  if (slot >= 0 && slot <= MAX_TIMERS) {
    if (tasks[slot] != NULL)
      tasks[slot]->enabled = true;
  }
}

void Timer::disableTimer(uint8_t slot) {
  if (slot >= 0 && slot <= MAX_TIMERS)
    if (tasks[slot] != NULL)
      tasks[slot]->enabled = false;
}

void Timer::deleteTimer(uint8_t slot) {
  if (slot >= 0 && slot <= MAX_TIMERS)
    if (tasks[slot] != NULL) {
      delete tasks[slot];
      tasks[slot] = NULL;
    }
}

void Timer::update() {
  unsigned int updateTime = millis() - lastUpdate;
  lastUpdate = millis();

  for (int8_t i = 0; i < MAX_TIMERS; i++) {
    if ((tasks[i] == NULL) || !tasks[i]->enabled)
      continue;

    struct TimerTask *task = tasks[i];
    if (task->timeLeft <= updateTime) {
      task->function();
      if (task->interval != 0) {
        task->timeLeft = task->interval;
      } else {
        deleteTimer(i);
      }
    } else {
      task->timeLeft -= updateTime;
    }
  }
}

void Timer::deleteAllTimers() {
  for (uint8_t i = 0; i < MAX_TIMERS; i++)
    deleteTimer(i);
}

int8_t Timer::findTimerSlot() {
  for (int8_t i = 0; i < MAX_TIMERS; i++) {
    if (tasks[i] == NULL)
      return i;
  }
  return -1;
}
