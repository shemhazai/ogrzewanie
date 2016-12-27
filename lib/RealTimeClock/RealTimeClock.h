/* DS3231  Real Time Clock */

#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include <Arduino.h>
#include <DateTime.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

#ifndef TWI_FREQ
#define TWI_FREQ 400000L
#endif

#define DS3231_ADDR_R 0xD1
#define DS3231_ADDR_W 0xD0
#define DS3231_ADDR 0x68

class RealTimeClock {
public:
  RealTimeClock(uint8_t data_pin, uint8_t sclk_pin);
  void begin();

  DateTime getDateTime();
  void setTime(uint8_t hour, uint8_t minute, uint8_t second);
  void setDate(uint8_t days, uint8_t month, uint16_t year);
  void setDayOfWeek(uint8_t dayOfWeek);

  void enable32KHz(bool enable);
  void setOutput(byte enable);
  void setSQWRate(int rate);
  float getTempC();

private:
  uint8_t _scl_pin;
  uint8_t _sda_pin;
  uint8_t _burstArray[7];
  boolean _use_hw;

  void _sendStart(byte addr);
  void _sendStop();
  void _sendAck();
  void _sendNack();
  void _waitForAck();
  uint8_t _readByte();
  void _writeByte(uint8_t value);
  void _burstRead();
  uint8_t _readRegister(uint8_t reg);
  void _writeRegister(uint8_t reg, uint8_t value);
  uint8_t _decode(uint8_t value);
  uint8_t _decodeH(uint8_t value);
  uint8_t _decodeY(uint8_t value);
  uint8_t _encode(uint8_t vaule);
};
#endif
