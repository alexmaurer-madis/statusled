/**
 * @file statusled.h
 *
 * Library to drive a status LED like off/on, blinking, counting, flashing...
 * Every project generally includes one or more status LED.
 * This library helps you to easily display the state of your device
 *
 * @author Alexandre Maurer (alexmaurer@madis.ch)
 * @license MIT
 *
 */

#ifndef __STATUSLED_H
#define __STATUSLED_H

#include <cstdint>

#define STATUSLED_STOP 0x01
#define STATUSLED_STILL 0x02
#define STATUSLED_BLINK 0x03
#define STATUSLED_COUNT 0x04
#define STATUSLED_FLASH 0x05

class StatusLed {
 public:
  StatusLed(void);
  StatusLed(uint32_t callsPerSecond);

  uint8_t process(void);
  uint8_t process(unsigned long millis);

  void ledSetStill(uint8_t state);
  void ledSetBlink(double period, double dutyCycle);
  void ledSetCount(uint8_t count, double onTime, double delay, double pause);
  void ledSetFlash(double onTime);

  void tick(void);

  uint8_t state;

 private:
  uint32_t _secToTicks(double time);

  volatile uint32_t _ticks;
  unsigned long _last_millis;

  uint8_t _ledProcess();
  uint32_t _callsPerSecond;
  uint8_t _oldState;
  uint8_t _stillState;

  uint32_t _onTicks;
  uint32_t _offTicks;
  uint32_t _pauseTicks;
  uint8_t _totalCount;
  uint8_t _currentCount;

  void (StatusLed::*_functionPtr)(void);
  void _ledFunctionStop();
  void _ledFunctionStill();
  void _ledFunctionBlink();
  void _ledFunctionCount();
  void _ledFunctionFlash();
};

#endif