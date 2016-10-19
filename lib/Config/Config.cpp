#include "Config.h"

bool Config::shouldTurnOnPKW() {
  return ((tkw >= 46) && (tskw >= 110)) || tkw >= 95;
}

bool Config::shouldTurnOffPKW() { return tskw <= 100 && tkw <= 93; }

bool Config::shouldTurnOnPCWU() {
  return (tb >= (tcwu + maxrb)) && (tcwu <= (ztcwu - tcwuh));
}

bool Config::shouldTurnOffPCWU() {
  return (tb <= (tcwu + minrb)) || (tcwu >= (ztcwu + tcwuh));
}

bool Config::shouldTurnOnPCO() { return tz < 17; }

bool Config::shouldTurnOffPCO() { return tz >= 18; }

bool Config::shouldOpenZT() { return (tco <= (tkg - zth)); }

bool Config::shouldCloseZT() { return (tco >= (tkg + zth)); }
