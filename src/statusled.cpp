/**
 * @file statusled.cpp
 *
 * Library to drive a status LED like off/on, blinking, counting, flashing...
 * Every project generally includes one or more status LED.
 * This library helps you to easily display the state of your device
 *
 * @author Alexandre Maurer (alexmaurer@madis.ch)
 * @license MIT
 *
 */

#include "statusled.h"

/**
 * @brief Construct a new StatusLed for use by calling process() within loop()
 * and using millis() as
 * the time base (preferred way for Arduino)
 *
 */
StatusLed::StatusLed(void) {
  calls_per_second_ = 1000;
  function_ptr_ = &StatusLed::ledFunctionStop;
}

/**
 * @brief Construct a new StatusLed to be used with a timer calling tick() at
 * regular interval
 *
 * @param calls_per_second specify the number of time per second tick() will be
 * called
 */
StatusLed::StatusLed(uint32_t calls_per_second) {
  calls_per_second_ = calls_per_second;
  function_ptr_ = &StatusLed::ledFunctionStop;
}

/**
 * @brief Function to be called within your loop() when using millis() as the
 * timebase.
 *
 * @param current_millis the current value that the function millis() returned
 * @return uint8_t return true if the state of the led must be changed
 */
uint8_t StatusLed::process(const unsigned long current_millis) {
  ticks_ += (unsigned long)(current_millis - last_millis_);
  last_millis_ = current_millis;
  return ledProcess();
}

/**
 * @brief Function to be called within your loop() when using timer and tick()
 *
 * @return uint8_t
 */
uint8_t StatusLed::process() { return ledProcess(); }

/**
 * @brief Function to be called by a timer at regular interval
 *
 */
void StatusLed::tick(void) { ticks_++; }

/**
 * @brief set led ON or OFF
 *
 * @param state true or 1 for ON, false or 0 for OFF
 */
void StatusLed::ledSetStill(const uint8_t state) {
  function_ptr_ = nullptr;
  still_state_ = state;
  function_ptr_ = &StatusLed::ledFunctionStill;
}

/**
 * @brief set blinking mode
 *
 * @param period in second from 0.01 up to 10s
 * @param duty_cycle from 10 to 90%
 */
void StatusLed::ledSetBlink(double period, double duty_cycle) {
  if (period < 0.01)
    period = 0.01;
  else if (period > 10)
    period = 10;

  if (duty_cycle < 10)
    duty_cycle = 10;
  else if (duty_cycle > 90)
    duty_cycle = 90;

  uint32_t blinkPeriodTicks = secToTicks(period);
  uint32_t blinkOnTicks = (uint32_t)(blinkPeriodTicks * (duty_cycle / 100));
  uint32_t blinkOffTicks = blinkPeriodTicks - blinkOnTicks;

  /* If same function, compare the parameters */
  if (function_ptr_ == &StatusLed::ledFunctionBlink) {
    /* If same parameters, do nothing */
    if ((blinkOnTicks == on_ticks_) && (blinkOffTicks == off_ticks_)) {
      return;
    }
  }

  function_ptr_ = &StatusLed::ledFunctionStop;
  on_ticks_ = blinkOnTicks;
  off_ticks_ = blinkOffTicks;
  ticks_ = 0;
  state = 0;
  function_ptr_ = &StatusLed::ledFunctionBlink;
}

/**
 * @brief Set counting mode
 *
 * @param count the number of time the led will blink before a pause (from 1 to
 * 20).
 * @param on_time duration in second when the led is ON (from 0.01 to 2s).
 * @param delay delay between blinking (from 0.01 to 4s).
 * @param pause the time in second to wait before blinking the led x times
 * again (from 1 to 10s).
 */
void StatusLed::ledSetCount(uint8_t count, double on_time, double delay,
                            double pause) {
  if (count < 1)
    count = 1;
  else if (count > 20)
    count = 20;

  if (on_time < 0.01)
    on_time = 0.01;
  else if (on_time > 20)
    on_time = 20;

  if (delay < 0.01)
    delay = 0.01;
  else if (delay > 4)
    delay = 4;

  if (pause < 1)
    pause = 1;
  else if (pause > 10)
    pause = 10;

  uint32_t countOnTicks = secToTicks(on_time);
  uint32_t countOffTicks = secToTicks(delay);
  uint32_t countPauseTicks = secToTicks(pause);

  /* If same function, compare the parameters */
  if (function_ptr_ == &StatusLed::ledFunctionCount) {
    /* If same parameters, do nothing */
    if ((on_ticks_ == countOnTicks) && (off_ticks_ == countOffTicks) &&
        (pause_ticks_ == countPauseTicks) && (total_count_ == count)) {
      return;
    }
  }

  function_ptr_ = &StatusLed::ledFunctionStop;
  on_ticks_ = countOnTicks;
  off_ticks_ = countOffTicks;
  pause_ticks_ = countPauseTicks;
  total_count_ = count;
  current_count_ = count;
  ticks_ = 0;
  state = 0;
  function_ptr_ = &StatusLed::ledFunctionCount;
}

/**
 * @brief flash the led 1 time.
 *
 * @param on_time Duration in second the led must be ON (from 0.01 to 1s)
 */
void StatusLed::ledSetFlash(double on_time) {
  if (on_time < 0.01)
    on_time = 0.01;
  else if (on_time > 1)
    on_time = 1;

  function_ptr_ = &StatusLed::ledFunctionStop;
  on_ticks_ = secToTicks(on_time);

  ticks_ = 0;
  state = 0;
  function_ptr_ = &StatusLed::ledFunctionFlash;
}

uint8_t StatusLed::ledProcess() {
  old_state_ = state;

  if (function_ptr_ != nullptr)
    (this->*function_ptr_)();

  return (old_state_ != state) ? 1 : 0;
}

void StatusLed::ledFunctionStop() {
  // Do nothing
}

void StatusLed::ledFunctionStill() { state = still_state_; }

void StatusLed::ledFunctionBlink() {
  // Reset ticks counter after period is elapsed
  if (ticks_ >= (on_ticks_ + off_ticks_))
    ticks_ = 0;

  state = (ticks_ >= on_ticks_) ? 0 : 1;
}

void StatusLed::ledFunctionCount() {
  // Pause before counting again
  if (current_count_ == 0) {
    if (ticks_ >= pause_ticks_) {
      // Reset ticks and reload counter
      ticks_ = 0;
      current_count_ = total_count_;
    } else
      return;
  }

  // Reset ticks counter after period is elapsed
  if (ticks_ >= (on_ticks_ + off_ticks_)) {
    ticks_ = 0;
    current_count_--;
    return;
  }

  state = (ticks_ < on_ticks_) ? 1 : 0;
}

/**
 * @brief Flash during the specified time and
 *
 */
void StatusLed::ledFunctionFlash() { state = (ticks_ >= on_ticks_) ? 0 : 1; }

/**
 * @brief * Convert from seconds to Ticks
 *
 * @param time Seconds to convert
 * @return uint32_t corresponding number of ticks
 */
uint32_t StatusLed::secToTicks(double time) {
  return (uint32_t)(time * (double)calls_per_second_);
}