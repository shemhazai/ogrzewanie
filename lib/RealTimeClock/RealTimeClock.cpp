/* DS3231  Real Time Clock */

#include "RealTimeClock.h"
#include <DateTime.h>

#define REG_SEC 0x00
#define REG_MIN 0x01
#define REG_HOUR 0x02
#define REG_DOW 0x03
#define REG_DATE 0x04
#define REG_MON 0x05
#define REG_YEAR 0x06
#define REG_CON 0x0e
#define REG_STATUS 0x0f
#define REG_AGING 0x10
#define REG_TEMPM 0x11
#define REG_TEMPL 0x12

RealTimeClock::RealTimeClock(uint8_t data_pin, uint8_t sclk_pin) {
  _sda_pin = data_pin;
  _scl_pin = sclk_pin;
  begin();
}

DateTime RealTimeClock::getDateTime() {
  DateTime t;
  _burstRead();
  t.second = _decode(_burstArray[0]);
  t.minute = _decode(_burstArray[1]);
  t.hour = _decodeH(_burstArray[2]);
  t.dayOfWeek = _burstArray[3];
  t.day = _decode(_burstArray[4]);
  t.month = _decode(_burstArray[5]);
  t.year = _decodeY(_burstArray[6]) + 2000;
  return t;
}

void RealTimeClock::setTime(uint8_t hour, uint8_t minute, uint8_t second) {
  if (((hour >= 0) && (hour < 24)) && ((minute >= 0) && (minute < 60)) &&
      ((second >= 0) && (second < 60))) {
    _writeRegister(REG_HOUR, _encode(hour));
    _writeRegister(REG_MIN, _encode(minute));
    _writeRegister(REG_SEC, _encode(second));
  }
}

void RealTimeClock::setDate(uint8_t days, uint8_t month, uint16_t year) {
  if (((days > 0) && (days <= 31)) && ((month > 0) && (month <= 12)) &&
      ((year >= 2000) && (year < 3000))) {
    year -= 2000;
    _writeRegister(REG_YEAR, _encode(year));
    _writeRegister(REG_MON, _encode(month));
    _writeRegister(REG_DATE, _encode(days));
  }
}

void RealTimeClock::setDayOfWeek(uint8_t dayOfWeek) {
  if ((dayOfWeek > 0) && (dayOfWeek < 8))
    _writeRegister(REG_DOW, dayOfWeek);
}

void RealTimeClock::enable32KHz(bool enable) {
  uint8_t _reg = _readRegister(REG_STATUS);
  _reg &= ~(1 << 3);
  _reg |= (enable << 3);
  _writeRegister(REG_STATUS, _reg);
}

void RealTimeClock::setOutput(byte enable) {
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(1 << 2);
  _reg |= (enable << 2);
  _writeRegister(REG_CON, _reg);
}

void RealTimeClock::setSQWRate(int rate) {
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(3 << 3);
  _reg |= (rate << 3);
  _writeRegister(REG_CON, _reg);
}

float RealTimeClock::getTempC() {
  uint8_t _msb = _readRegister(REG_TEMPM);
  uint8_t _lsb = _readRegister(REG_TEMPL);
  return (float)_msb + ((_lsb >> 6) * 0.25f);
}

/* Private */

void RealTimeClock::_sendStart(byte addr) {
  pinMode(_sda_pin, OUTPUT);
  digitalWrite(_sda_pin, HIGH);
  digitalWrite(_scl_pin, HIGH);
  digitalWrite(_sda_pin, LOW);
  digitalWrite(_scl_pin, LOW);
  shiftOut(_sda_pin, _scl_pin, MSBFIRST, addr);
}

void RealTimeClock::_sendStop() {
  pinMode(_sda_pin, OUTPUT);
  digitalWrite(_sda_pin, LOW);
  digitalWrite(_scl_pin, HIGH);
  digitalWrite(_sda_pin, HIGH);
  pinMode(_sda_pin, INPUT);
}

void RealTimeClock::_sendNack() {
  pinMode(_sda_pin, OUTPUT);
  digitalWrite(_scl_pin, LOW);
  digitalWrite(_sda_pin, HIGH);
  digitalWrite(_scl_pin, HIGH);
  digitalWrite(_scl_pin, LOW);
  pinMode(_sda_pin, INPUT);
}

void RealTimeClock::_sendAck() {
  pinMode(_sda_pin, OUTPUT);
  digitalWrite(_scl_pin, LOW);
  digitalWrite(_sda_pin, LOW);
  digitalWrite(_scl_pin, HIGH);
  digitalWrite(_scl_pin, LOW);
  pinMode(_sda_pin, INPUT);
}

void RealTimeClock::_waitForAck() {
  pinMode(_sda_pin, INPUT);
  digitalWrite(_scl_pin, HIGH);
  while (digitalRead(_sda_pin) == HIGH)
    continue;

  digitalWrite(_scl_pin, LOW);
}

uint8_t RealTimeClock::_readByte() {
  pinMode(_sda_pin, INPUT);

  uint8_t value = 0;
  uint8_t currentBit = 0;

  for (int i = 0; i < 8; ++i) {
    digitalWrite(_scl_pin, HIGH);
    currentBit = digitalRead(_sda_pin);
    value |= ((currentBit << 7) - i);
    delayMicroseconds(1);
    digitalWrite(_scl_pin, LOW);
  }
  return value;
}

void RealTimeClock::_writeByte(uint8_t value) {
  pinMode(_sda_pin, OUTPUT);
  shiftOut(_sda_pin, _scl_pin, MSBFIRST, value);
}

uint8_t RealTimeClock::_decode(uint8_t value) {
  uint8_t decoded = value & 127;
  decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
  return decoded;
}

uint8_t RealTimeClock::_decodeH(uint8_t value) {
  if (value & 128)
    value = (value & 15) + (12 * ((value & 32) >> 5));
  else
    value = (value & 15) + (10 * ((value & 48) >> 4));
  return value;
}

uint8_t RealTimeClock::_decodeY(uint8_t value) {
  uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
  return decoded;
}

uint8_t RealTimeClock::_encode(uint8_t value) {
  uint8_t encoded = ((value / 10) << 4) + (value % 10);
  return encoded;
}

void RealTimeClock::begin() {
  if ((_sda_pin == SDA) and (_scl_pin == SCL)) {
    _use_hw = true;
    // activate internal pullups for twi.
    digitalWrite(SDA, HIGH);
    digitalWrite(SCL, HIGH);
    // delay(1);  // Workaround for a linker bug

    // initialize twi prescaler and bit rate
    cbi(TWSR, TWPS0);
    cbi(TWSR, TWPS1);
    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

    // enable twi module, acks, and twi interrupt
    TWCR = _BV(TWEN) | _BV(TWIE) /* | _BV(TWEA)*/;
  } else {
    _use_hw = false;
    pinMode(_scl_pin, OUTPUT);
  }
}

void RealTimeClock::_burstRead() {
  if (_use_hw) {
    // Send start address
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA); // Send START
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = DS3231_ADDR_W;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = 0;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready

    // Read data starting from start address
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA); // Send rep. START
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = DS3231_ADDR_R;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    for (int i = 0; i < 7; i++) {
      TWCR = _BV(TWEN) | _BV(TWINT) |
             _BV(TWEA); // Send ACK and clear TWINT to proceed
      while ((TWCR & _BV(TWINT)) == 0) {
      }; // Wait for TWI to be ready
      _burstArray[i] = TWDR;
    }
    TWCR = _BV(TWEN) | _BV(TWINT); // Send NACK and clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready

    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); // Send STOP
  } else {
    _sendStart(DS3231_ADDR_W);
    _waitForAck();
    _writeByte(0);
    _waitForAck();
    _sendStart(DS3231_ADDR_R);
    _waitForAck();

    for (int i = 0; i < 7; i++) {
      _burstArray[i] = _readByte();
      if (i < 6)
        _sendAck();
      else
        _sendNack();
    }
    _sendStop();
  }
}

uint8_t RealTimeClock::_readRegister(uint8_t reg) {
  uint8_t readValue = 0;

  if (_use_hw) {
    // Send start address
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA); // Send START
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = DS3231_ADDR_W;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = reg;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready

    // Read data starting from start address
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA); // Send rep. START
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = DS3231_ADDR_R;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWCR = _BV(TWEN) | _BV(TWINT) |
           _BV(TWEA); // Send ACK and clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    readValue = TWDR;
    TWCR = _BV(TWEN) | _BV(TWINT); // Send NACK and clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready

    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); // Send STOP
  } else {
    _sendStart(DS3231_ADDR_W);
    _waitForAck();
    _writeByte(reg);
    _waitForAck();
    _sendStart(DS3231_ADDR_R);
    _waitForAck();
    readValue = _readByte();
    _sendNack();
    _sendStop();
  }
  return readValue;
}

void RealTimeClock::_writeRegister(uint8_t reg, uint8_t value) {
  if (_use_hw) {
    // Send start address
    TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA); // Send START
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = DS3231_ADDR_W;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = reg;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready
    TWDR = value;
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA); // Clear TWINT to proceed
    while ((TWCR & _BV(TWINT)) == 0) {
    }; // Wait for TWI to be ready

    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO); // Send STOP
  } else {
    _sendStart(DS3231_ADDR_W);
    _waitForAck();
    _writeByte(reg);
    _waitForAck();
    _writeByte(value);
    _waitForAck();
    _sendStop();
  }
}
