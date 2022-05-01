#include "statusled.h"

StatusLed::StatusLed(uint32_t callsPerSecond) {
  _callsPerSecond=callsPerSecond;
  _function=STATUSLED_STOP;
}

void StatusLed::tick(void) {
  _ticks++;
}


void StatusLed::ledSetStill(uint32_t state)
{
  _function = STATUSLED_STILL;
  state = state;
}


void StatusLed::ledSetBlink(double period, double dutyCycle){
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

void StatusLed::ledSetCount(uint32_t count, double onTime, double delay, double sleep)
{
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

uint8_t StatusLed::ledProcess() {
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