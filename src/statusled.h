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

#include "Arduino.h"

#include <cstdint>
#include <map>

class StatusLed {
public:
  StatusLed(void);
  StatusLed(uint32_t calls_per_second);

  uint8_t process(void);
  uint8_t process(unsigned long millis);
  void tick(void);

  void ledSetStill(uint8_t state);
  void ledSetBlink(double period, double duty_cycle);
  void ledSetCount(uint8_t count, double on_time, double delay, double pause);
  void ledSetFlash(double on_time);

  uint8_t state;

  uint8_t pin_;
  bool invert_;

private:
  uint32_t secToTicks(double time);
  uint8_t ledProcess();

  volatile uint32_t ticks_;
  unsigned long last_millis_;
  bool function_changed_;

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

class StatusLedManager {
public:
  StatusLedManager(void);
  StatusLedManager(uint32_t calls_per_second);
  void createStatusLed(const char *name, uint8_t pin, bool invert = false);

  StatusLed &operator()(const char *name);

  void process(unsigned long millis);
  void tick(void);

private:
  typedef struct StatusLedStruct {
    StatusLed *sl;
    uint8_t pin;
    bool invert = false;
  } StatusLedStruct;

  uint32_t calls_per_second_;
  std::map<const char *, StatusLedStruct> leds_;
};

#endif