#ifndef CONDITION_H
#define CONDITION_H

#include "config.h"

inline bool shouldTurnOnPKW(struct Config &conf) {
  return conf.tkw >= 40 && conf.tskw >= 110;
}

inline bool shouldTurnOffPKW(struct Config &conf) { return conf.tskw <= 100; }

inline bool shouldTurnOnPCWU(struct Config &conf) {
  return (conf.tb >= (conf.tcwu + conf.maxrb)) &&
         (conf.tcwu <= (conf.ztcwu - conf.tcwuh));
}

inline bool shouldTurnOffPCWU(struct Config &conf) {
  return ((conf.tb <= conf.tcwu) + conf.minrb) ||
         (conf.tcwu >= (conf.ztcwu + conf.tcwuh));
}

inline bool shouldTurnOnPCO(struct Config &conf) { return conf.tz < 17; }

inline bool shouldTurnOffPCO(struct Config &conf) { return conf.tz >= 18; }

inline bool shouldOpenZT(struct Config &conf) {
  return conf.tco <= conf.tkg - conf.zth;
}

inline bool shouldCloseZT(struct Config &conf) {
  return conf.tco >= conf.tkg + conf.zth;
}

#endif
