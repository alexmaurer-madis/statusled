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
  StatusLed(uint32_t calls_per_second);

  uint8_t process(void);
  uint8_t process(unsigned long millis);

  void ledSetStill(uint8_t state);
  void ledSetBlink(double period, double duty_cycle);
  void ledSetCount(uint8_t count, double on_time, double delay, double pause);
  void ledSetFlash(double on_time);

  void tick(void);

  uint8_t state;

private:
  uint32_t secToTicks(double time);

  volatile uint32_t ticks_;
  unsigned long last_millis_;

  uint8_t ledProcess();
  uint32_t calls_per_second_;
  uint8_t old_state_;
  uint8_t still_state_;

  uint32_t on_ticks_;
  uint32_t off_ticks_;
  uint32_t pause_ticks_;
  uint8_t total_count_;
  uint8_t current_count_;

  void (StatusLed::*function_ptr_)(void);
  void ledFunctionStop();
  void ledFunctionStill();
  void ledFunctionBlink();
  void ledFunctionCount();
  void ledFunctionFlash();
};

#endif