#include "statusled.h"

#include <stdint.h>

/**
 * @brief Construct a new Status Led for use within loop() and using millis() as the time base
 * 
 */
StatusLed::StatusLed(void) {
  _callsPerSecond=1000;
  _function=STATUSLED_STOP;
}

/**
 * @brief Construct a new Status Led to be used with a timer calling tick()at regular interval
 * 
 * @param callsPerSecond specify the number of time per second tick() will be called
 */
StatusLed::StatusLed(uint32_t callsPerSecond) {
  _callsPerSecond=callsPerSecond;
  _function=STATUSLED_STOP;
}

/**
 * @brief Function to be called within your loop() when using millis()
 * 
 * @param current_millis the current value of function millis()
 * @return uint8_t return true if the state of led must be changed
 */
uint8_t StatusLed::process(unsigned long current_millis) {
  _ticks+= (unsigned long)(current_millis - _last_millis);
  _last_millis=current_millis;
  return _ledProcess();
}

/**
 * @brief Function to be called within your loop() when using timer and tick()
 * 
 * @return uint8_t 
 */
uint8_t StatusLed::process() {
  return _ledProcess();
}


/**
 * @brief Function to be called by a timer at regular interval
 * 
 */
void StatusLed::tick(void) {
  _ticks++;
}

/**
 * @brief set led ON or OFF
 * 
 * @param state true for ON, false for OFF
 */
void StatusLed::ledSetStill(uint8_t state)
{
  _function = STATUSLED_STILL;
  state = state;
  
}

/**
 * @brief set blinking mode
 * 
 * @param period in second from 0.01 up to 10s
 * @param dutyCycle from 10 to 90%
 */
void StatusLed::ledSetBlink(double period, double dutyCycle){
  
  if(period<0.01)
    period=0.01;
  else if(period>10)
    period=10;

  if(dutyCycle <10 )
    dutyCycle=10;
  else if(dutyCycle >90)
    dutyCycle=90;

  uint32_t blinkPeriodTicks = timeToTicks(period);
  uint32_t blinkOnTicks = (uint32_t)(blinkPeriodTicks*(dutyCycle/100));
  
  /* If same function, compare the parameters */
  if(_function == STATUSLED_BLINK)
  {
    /* If same parameters, do nothing */
    if((_blinkParam.blinkPeriodTicks == blinkPeriodTicks) &&
       (_blinkParam.blinkOnTicks = blinkOnTicks)) {
         return;
       }
  }
  
  _function = STATUSLED_STOP; 
  _blinkParam.blinkPeriodTicks = blinkPeriodTicks;
  _blinkParam.blinkOnTicks = blinkOnTicks;
  _ticks=0;
  state=0;
  _function = STATUSLED_BLINK;
}

/**
 * @brief Set counting mode
 * 
 * @param count N time the led will blink before a pause. From 1 to 20.
 * @param onTime duration in second when the led is ON. From 0.01 to 2s.
 * @param delay delay between blinking. From 0.01 to 4s.
 * @param sleep the time in second to wait before blinking the led N times again. From 1 to 10s.
 */
void StatusLed::ledSetCount(uint32_t count, double onTime, double delay, double sleep)
{
  if(count <1)
    count=1;
  else if(count>20)
    count=20;

  if(onTime<0.01)
    onTime=0.01;
  else if(onTime>20)
    onTime=20;

  if(delay<0.01)
    delay=0.01;
  else if(delay>4)
    delay=4;

  if(sleep<1)
    sleep=1;
  else if(sleep>10)
    sleep=10;

  uint32_t countOnTicks = timeToTicks(onTime);
  uint32_t countOffTicks = timeToTicks(delay);
  uint32_t countPauseTicks = timeToTicks(sleep);
  
  /* If same function, compare the parameters */
  if(_function == STATUSLED_COUNT)
  {
    /* If same parameters, do nothing */
    if((_countParam.countOnTicks == countOnTicks) && 
       (_countParam.countOffTicks == countOffTicks) &&
        (_countParam.countPauseTicks == countPauseTicks) &&
        (_countParam.count == count) ){
      return;
    }
  }

  _function = STATUSLED_STOP;
  _countParam.countOnTicks = countOnTicks;
  _countParam.countOffTicks = countOffTicks;
  _countParam.countPauseTicks = countPauseTicks;
  _countParam.count = count;
  _countParam.currentCount = count;
  _ticks=0;
  state=0;
  _function = STATUSLED_COUNT;
}

void StatusLed::ledSetFlash(double onTime) {
  uint32_t onTicks = timeToTicks(onTime);
    
  _function = STATUSLED_STOP; 
  _flashParam.onTicks = onTicks;

  _ticks=0;
  state=0;
  _function = STATUSLED_FLASH;
}

uint8_t StatusLed::_ledProcess() {
  _oldState = state;

  switch(_function)
  {
  case STATUSLED_STILL:
    _ledFunctionStill();
    break;
    
  case STATUSLED_BLINK:
    _ledFunctionBlink();
    break;
      
  case STATUSLED_COUNT:
    _ledFunctionCount();
    break;

  case STATUSLED_FLASH:
    _ledFunctionFlash();
    break;
    
  case STATUSLED_STOP:
    //Do nothing
    break;
    
  default:
    break;
  }

  return (_oldState != state) ? 1: 0;
}

void StatusLed::_ledFunctionStill()
{
  //Do nothing
}

void StatusLed::_ledFunctionBlink()
{
  /* Reset ticks counter (period) */
  if(_ticks >= _blinkParam.blinkPeriodTicks)
    _ticks = 0;
  
  state = (_ticks >= _blinkParam.blinkOnTicks) ? 0 : 1;
}

void StatusLed::_ledFunctionCount() {
  //Pause ?
  if(_countParam.currentCount == 0)
  {
    if(_ticks >= _countParam.countPauseTicks)
    {
      //Reset ticks and reload counter
      _ticks = 0;
      _countParam.currentCount = _countParam.count;
    }
    else
      return;
  }
  
  //Reset ticks counter (period)
  if(_ticks >= (_countParam.countOnTicks + _countParam.countOffTicks))
  {
     _ticks = 0;
     _countParam.currentCount--;
     return;
  }
  
  state  = (_ticks < _countParam.countOnTicks) ? 1 : 0;
}

void StatusLed::_ledFunctionFlash() {
  state = (_ticks >= _flashParam.onTicks) ? 0 : 1;
}

/*
 * Convert time to Ticks
 */
uint32_t StatusLed::timeToTicks(double time) {
  return (uint32_t)(time * (double)_callsPerSecond);
}