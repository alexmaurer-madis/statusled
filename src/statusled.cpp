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
  _callsPerSecond = 1000;
  _functionPtr = &StatusLed::_ledFunctionStop;
}

/**
 * @brief Construct a new StatusLed to be used with a timer calling tick() at
 * regular interval
 *
 * @param callsPerSecond specify the number of time per second tick() will be
 * called
 */
StatusLed::StatusLed(uint32_t callsPerSecond) {
  _callsPerSecond = callsPerSecond;
  _functionPtr = &StatusLed::_ledFunctionStop;
}

/**
 * @brief Function to be called within your loop() when using millis() as the
 * timebase.
 *
 * @param current_millis the current value that the function millis() returned
 * @return uint8_t return true if the state of the led must be changed
 */
uint8_t StatusLed::process(const unsigned long current_millis) {
  _ticks += (unsigned long)(current_millis - _last_millis);
  _last_millis = current_millis;
  return _ledProcess();
}

/**
 * @brief Function to be called within your loop() when using timer and tick()
 *
 * @return uint8_t
 */
uint8_t StatusLed::process() { return _ledProcess(); }

/**
 * @brief Function to be called by a timer at regular interval
 *
 */
void StatusLed::tick(void) { _ticks++; }

/**
 * @brief set led ON or OFF
 *
 * @param state true or 1 for ON, false or 0 for OFF
 */
void StatusLed::ledSetStill(const uint8_t state) {
  _functionPtr = nullptr;
  _stillState = state;
  _functionPtr = &StatusLed::_ledFunctionStill;
}

/**
 * @brief set blinking mode
 *
 * @param period in second from 0.01 up to 10s
 * @param dutyCycle from 10 to 90%
 */
void StatusLed::ledSetBlink(double period, double dutyCycle) {
  if (period < 0.01)
    period = 0.01;
  else if (period > 10)
    period = 10;

  if (dutyCycle < 10)
    dutyCycle = 10;
  else if (dutyCycle > 90)
    dutyCycle = 90;

  uint32_t blinkPeriodTicks = _secToTicks(period);
  uint32_t blinkOnTicks = (uint32_t)(blinkPeriodTicks * (dutyCycle / 100));
  uint32_t blinkOffTicks = blinkPeriodTicks - blinkOnTicks;

  /* If same function, compare the parameters */
  if (_functionPtr == &StatusLed::_ledFunctionBlink) {
    /* If same parameters, do nothing */
    if ((blinkOnTicks == _onTicks) && (blinkOffTicks == _offTicks)) {
      return;
    }
  }

  _functionPtr = &StatusLed::_ledFunctionStop;
  _onTicks = blinkOnTicks;
  _offTicks = blinkOffTicks;
  _ticks = 0;
  state = 0;
  _functionPtr = &StatusLed::_ledFunctionBlink;
}

/**
 * @brief Set counting mode
 *
 * @param count the number of time the led will blink before a pause (from 1 to
 * 20).
 * @param onTime duration in second when the led is ON (from 0.01 to 2s).
 * @param delay delay between blinking (from 0.01 to 4s).
 * @param pause the time in second to wait before blinking the led x times
 * again (from 1 to 10s).
 */
void StatusLed::ledSetCount(uint8_t count, double onTime, double delay,
                            double pause) {
  if (count < 1)
    count = 1;
  else if (count > 20)
    count = 20;

  if (onTime < 0.01)
    onTime = 0.01;
  else if (onTime > 20)
    onTime = 20;

  if (delay < 0.01)
    delay = 0.01;
  else if (delay > 4)
    delay = 4;

  if (pause < 1)
    pause = 1;
  else if (pause > 10)
    pause = 10;

  uint32_t countOnTicks = _secToTicks(onTime);
  uint32_t countOffTicks = _secToTicks(delay);
  uint32_t countPauseTicks = _secToTicks(pause);

  /* If same function, compare the parameters */
  if (_functionPtr == &StatusLed::_ledFunctionCount) {
    /* If same parameters, do nothing */
    if ((_onTicks == countOnTicks) && (_offTicks == countOffTicks) &&
        (_pauseTicks == countPauseTicks) && (_totalCount == count)) {
      return;
    }
  }

  _functionPtr = &StatusLed::_ledFunctionStop;
  _onTicks = countOnTicks;
  _offTicks = countOffTicks;
  _pauseTicks = countPauseTicks;
  _totalCount = count;
  _currentCount = count;
  _ticks = 0;
  state = 0;
  _functionPtr = &StatusLed::_ledFunctionCount;
}

/**
 * @brief flash the led 1 time.
 *
 * @param onTime Duration in second the led must be ON (from 0.01 to 1s)
 */
void StatusLed::ledSetFlash(double onTime) {
  if (onTime < 0.01)
    onTime = 0.01;
  else if (onTime > 1)
    onTime = 1;

  _functionPtr = &StatusLed::_ledFunctionStop;
  _onTicks = _secToTicks(onTime);
  ;
  _ticks = 0;
  state = 0;
  _functionPtr = &StatusLed::_ledFunctionFlash;
}

uint8_t StatusLed::_ledProcess() {
  _oldState = state;

  if (_functionPtr != nullptr) (this->*_functionPtr)();

  return (_oldState != state) ? 1 : 0;
}

void StatusLed::_ledFunctionStop() {
  // Do nothing
}

void StatusLed::_ledFunctionStill() { state = _stillState; }

void StatusLed::_ledFunctionBlink() {
  // Reset ticks counter after period is elapsed
  if (_ticks >= (_onTicks + _offTicks)) _ticks = 0;

  state = (_ticks >= _onTicks) ? 0 : 1;
}

void StatusLed::_ledFunctionCount() {
  // Pause before counting again
  if (_currentCount == 0) {
    if (_ticks >= _pauseTicks) {
      // Reset ticks and reload counter
      _ticks = 0;
      _currentCount = _totalCount;
    } else
      return;
  }

  // Reset ticks counter after period is elapsed
  if (_ticks >= (_onTicks + _offTicks)) {
    _ticks = 0;
    _currentCount--;
    return;
  }

  state = (_ticks < _onTicks) ? 1 : 0;
}

/**
 * @brief Flash during the specified time and
 *
 */
void StatusLed::_ledFunctionFlash() { state = (_ticks >= _onTicks) ? 0 : 1; }

/**
 * @brief * Convert from seconds to Ticks
 *
 * @param time Seconds to convert
 * @return uint32_t corresponding number of ticks
 */
uint32_t StatusLed::_secToTicks(double time) {
  return (uint32_t)(time * (double)_callsPerSecond);
}