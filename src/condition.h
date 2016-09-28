#ifndef CONDITION_H
#define CONDITION_H

#include "config.h"

inline bool shouldTurnOnPKW(struct Config *conf) {
  return conf->tkw >= 40 && conf->tskw >= 100;
}

inline bool shouldTurnOffPKW(struct Config *conf) { return conf->tskw <= 100; }

inline bool shouldTurnOnPCWU(struct Config *conf) {
  const float HISTERESIS = 1;
  const float BUFFER_RESERVE = 5;
  return (conf->tb >= (conf->tcwu + BUFFER_RESERVE)) &&
         (conf->tcwu <= (conf->ztcwu - HISTERESIS));
}

inline bool shouldTurnOffPCWU(struct Config *conf) {
  const float HISTERESIS = 1;
  const float BUFFER_RESERVE = 2;
  return ((conf->tb <= conf->tcwu) + BUFFER_RESERVE) ||
         (conf->tcwu >= (conf->ztcwu + HISTERESIS));
}

inline bool shouldTurnOnPCO(struct Config *conf) { return conf->tz < 17; }

inline bool shouldTurnOffPCO(struct Config *conf) { return conf->tz >= 18; }

inline bool shouldOpenZT(struct Config *conf) {
  const float HISTERESIS = 1.5;
  return conf->tco <= conf->tkg - HISTERESIS;
}

inline bool shouldCloseZT(struct Config *conf) {
  const float HISTERESIS = 1.5;
  return conf->tco >= conf->tkg + HISTERESIS;
}

#endif
