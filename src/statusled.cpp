/**
 * @file statusled.cpp
 *
 * Library to drive a status LED with a specific pattern like blinking,
 * counting, flashing... Every project generally includes one or more status
 * LED. This library helps you to easily display the state of your device
 *
 * @author Alexandre Maurer (alexmaurer@madis.ch)
 * @license MIT
 *
 */

#include "statusled.h"

#ifndef STATUSLED_USE_MAP
#include <cstring>  // for strcmp
#endif

/**
 * @brief Construct a new StatusLed. Usage by calling process() within the
 * loop() function and using millis() as the time base (preferred way for
 * Arduino)
 *
 */
StatusLed::StatusLed(void) : StatusLed(1000) {}

/**
 * @brief Construct a new StatusLed to be used with a timer calling the method
 * tick() at fixed interval
 *
 * @param calls_per_second specify the number of time per second tick() will be
 * called
 */
StatusLed::StatusLed(uint32_t calls_per_second) {
  calls_per_second_ = calls_per_second;
  function_ptr_ = &StatusLed::ledFunctionStop;
  function_changed_ = true;
}

StatusLed::~StatusLed() {}

void StatusLed::setPin(uint8_t pin, bool invert = false) {
  pin_ = pin;
  invert_ = invert;
}

uint8_t StatusLed::getPin(void) { return pin_; }

bool StatusLed::getInvert(void) { return invert_; }

/**
 * @brief This function must be called within your loop() when using millis() as
 * the timebase.
 *
 * @param current_millis the current value that the function millis() returned
 * @return uint8_t return true if the state of the led must be changed
 */
uint8_t StatusLed::process(const unsigned long current_millis) {
  if (function_changed_) {
    function_changed_ = false;
    // Set last_millis_ on function change
    last_millis_ = current_millis;
  }

  ticks_ += (unsigned long)(current_millis - last_millis_);
  last_millis_ = current_millis;

  uint8_t old_state = state;

  if (function_ptr_ != nullptr)
    (this->*function_ptr_)();

  return (old_state != state) ? 1 : 0;
}

/**
 * @brief This function must be called within your loop() when using timer and
 * tick()
 *
 * @return uint8_t
 */
uint8_t StatusLed::process() {
  if (function_changed_) {
    function_changed_ = false;
  }

  uint8_t old_state = state;

  if (function_ptr_ != nullptr)
    (this->*function_ptr_)();

  return (old_state != state) ? 1 : 0;
}

/**
 * @brief Function to be called by a timer at fixed interval
 *
 */
void StatusLed::tick(void) { ticks_++; }

/**
 * @brief Set led ON or OFF
 *
 * @param state true or 1 for ON, false or 0 for OFF
 */
void StatusLed::ledSetStill(const uint8_t state) {
  function_ptr_ = nullptr;
  pattern_.still_state = state;
  function_ptr_ = &StatusLed::ledFunctionStill;
  function_changed_ = true;
}

/**
 * @brief Set blinking mode
 *
 * @param period in second from 0.01 up to 10s
 * @param duty_cycle from 10 to 90%
 * @param duration_ms duration in milliseconds (0 = blink indefinitely,
 *                    >0 = blink for the specified duration then stop).
 *                    Maximum value is 3600000 (1 hour).
 */
void StatusLed::ledSetBlink(double period, double duty_cycle,
                            uint32_t duration_ms) {
  if (period < 0.01)
    period = 0.01;
  else if (period > 10)
    period = 10;

  if (duty_cycle < 10)
    duty_cycle = 10;
  else if (duty_cycle > 90)
    duty_cycle = 90;

  if (duration_ms > 3600000)
    duration_ms = 3600000;

  uint32_t blinkPeriodTicks = secToTicks(period);
  uint32_t blinkOnTicks = (uint32_t)(blinkPeriodTicks * (duty_cycle / 100));
  uint32_t blinkOffTicks = blinkPeriodTicks - blinkOnTicks;
  uint32_t blinkDurationTicks =
      (uint32_t)((duration_ms * calls_per_second_) / 1000);

  /* If same function, compare the parameters */
  if (function_ptr_ == &StatusLed::ledFunctionBlink) {
    /* If same parameters, do nothing */
    if ((blinkOnTicks == on_ticks_) && (blinkOffTicks == off_ticks_) &&
        (blinkDurationTicks == pattern_.blink.duration_ticks)) {
      return;
    }
  }

  function_ptr_ = &StatusLed::ledFunctionStop;
  on_ticks_ = blinkOnTicks;
  off_ticks_ = blinkOffTicks;
  pattern_.blink.duration_ticks = blinkDurationTicks;
  ticks_ = 0;
  state = 0;
  function_ptr_ = &StatusLed::ledFunctionBlink;
  function_changed_ = true;
}

/**
 * @brief Set counting mode
 *
 * @param count the number of time the led will blink before a pause (from 1
 * to 20).
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
        (pattern_.count.pause_ticks == countPauseTicks) &&
        (pattern_.count.total_count == count)) {
      return;
    }
  }

  function_ptr_ = &StatusLed::ledFunctionStop;
  on_ticks_ = countOnTicks;
  off_ticks_ = countOffTicks;
  pattern_.count.pause_ticks = countPauseTicks;
  pattern_.count.total_count = count;
  pattern_.count.current_count = count;
  ticks_ = 0;
  state = 0;
  function_ptr_ = &StatusLed::ledFunctionCount;
  function_changed_ = true;
}

/**
 * @brief Flash the led 1 time.
 *
 * @param on_time Duration in second the led must be ON (minimum on time
 * enforced to 0.01s)
 */
void StatusLed::ledSetFlash(double on_time) {
  if (on_time < 0.01)
    on_time = 0.01;

  function_ptr_ = &StatusLed::ledFunctionStop;
  on_ticks_ = secToTicks(on_time);

  ticks_ = 0;
  state = 0;
  function_ptr_ = &StatusLed::ledFunctionFlash;
  function_changed_ = true;
}

// uint8_t StatusLed::ledProcess() {
//   uint8_t old_state = state;

//   if (function_ptr_ != nullptr)
//     (this->*function_ptr_)();

//   return (old_state != state) ? 1 : 0;
// }

void StatusLed::ledFunctionStop() {
  // Do nothing
}

void StatusLed::ledFunctionStill() { state = pattern_.still_state; }

void StatusLed::ledFunctionBlink() {
  // Check if duration has elapsed (only if duration_ticks > 0)
  if (pattern_.blink.duration_ticks > 0 &&
      ticks_ >= pattern_.blink.duration_ticks) {
    state = 0;
    function_ptr_ = &StatusLed::ledFunctionStop;
    return;
  }

  // Calculate position within current period
  uint32_t period_ticks = on_ticks_ + off_ticks_;
  uint32_t position_in_period = ticks_ % period_ticks;

  state = (position_in_period < on_ticks_) ? 1 : 0;
}

void StatusLed::ledFunctionCount() {
  // Pause before counting again
  if (pattern_.count.current_count == 0) {
    state = 0;  // Always OFF during pause
    if (ticks_ >= pattern_.count.pause_ticks) {
      // Reset ticks and reload counter
      ticks_ = 0;
      pattern_.count.current_count = pattern_.count.total_count;
    }
    return;
  }

  // Reset ticks counter after period is elapsed
  if (ticks_ >= (on_ticks_ + off_ticks_)) {
    ticks_ = 0;
    pattern_.count.current_count--;
    if (pattern_.count.current_count == 0) {
      state = 0;  // Enter pause
      return;
    }
  }

  state = (ticks_ < on_ticks_) ? 1 : 0;
}

/**
 * @brief Led ON during the specified time
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

/**
 * @brief Construct a new StatusLedManager. Usage by calling process() within
 * the loop() function and millis() as the time base (preferred way for Arduino)
 *
 */
StatusLedManager::StatusLedManager(void) {
  calls_per_second_ = 1000;
#ifndef STATUSLED_USE_MAP
  led_count_ = 0;
#endif
}

/**
 * @brief Construct a new StatusLedManager to be used with a timer calling
 * tick() at precise interval
 *
 * @param calls_per_second specify the number of time per second tick() will be
 * called
 */
StatusLedManager::StatusLedManager(const uint32_t calls_per_second) {
  calls_per_second_ = calls_per_second;
#ifndef STATUSLED_USE_MAP
  led_count_ = 0;
#endif
}

StatusLedManager::~StatusLedManager() {
#ifdef STATUSLED_USE_MAP
  for (auto &kv : leds_) {
    delete kv.second;
  }
#else
  for (uint8_t i = 0; i < led_count_; i++) {
    delete leds_[i].led;
  }
#endif
}

/**
 * @brief Create a new StatusLed that will be driven by the StatusLedManager.
 * It is up to you to set the correct pin as output.
 *
 * @param name Custom name for your led (for ex. system, ready, run, error...)
 * @param pin pin number of your led
 * @param invert Optional, True to invert output
 */
void StatusLedManager::createStatusLed(const char *name, uint8_t pin,
                                       const bool invert) {

  StatusLed *sl = new StatusLed(calls_per_second_);
  sl->setPin(pin, invert);

  digitalWrite(pin, invert ? !sl->state : sl->state);

#ifdef STATUSLED_USE_MAP
  leds_.insert(std::pair<const char *, StatusLed *>(name, sl));
#else
  if (led_count_ < STATUSLED_MAX_LEDS) {
    leds_[led_count_].name = name;
    leds_[led_count_].led = sl;
    led_count_++;
  }
#endif
}

/**
 * @brief Function to be called within your loop() when using millis() as the
 * timebase.
 *
 * @param millis the value that the function millis() returned
 */
void StatusLedManager::process(const unsigned long millis) {
#ifdef STATUSLED_USE_MAP
  for (auto &kv : leds_) {
    if (kv.second->process(millis)) {
#ifdef Arduino_h
      digitalWrite(kv.second->getPin(), kv.second->getInvert()
                                            ? !kv.second->state
                                            : kv.second->state);
#endif
    }
  }
#else
  for (uint8_t i = 0; i < led_count_; i++) {
    if (leds_[i].led->process(millis)) {
#ifdef Arduino_h
      digitalWrite(leds_[i].led->getPin(), leds_[i].led->getInvert()
                                               ? !leds_[i].led->state
                                               : leds_[i].led->state);
#endif
    }
  }
#endif
}

/**
 * @brief Function to be called by a timer at fixed interval
 *
 */
void StatusLedManager::tick(void) {
#ifdef STATUSLED_USE_MAP
  for (auto &kv : leds_) {
    kv.second->tick();
  }
#else
  for (uint8_t i = 0; i < led_count_; i++) {
    leds_[i].led->tick();
  }
#endif
}

/**
 * @brief Return the StatusLed object corresponding to the custom name
 *
 * @param name The custom name of your StatusLed
 * @return StatusLed&
 */
StatusLed &StatusLedManager::operator()(const char *name) {
#ifdef STATUSLED_USE_MAP
  return *leds_.at(name);
#else
  for (uint8_t i = 0; i < led_count_; i++) {
    if (strcmp(leds_[i].name, name) == 0) {
      return *leds_[i].led;
    }
  }
  // Return first LED if not found (should not happen in normal use)
  return *leds_[0].led;
#endif
}
