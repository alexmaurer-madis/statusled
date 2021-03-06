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

#include <stdint.h>

#define STATUSLED_STOP  0x01
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
    void ledSetCount(uint32_t count, double onTime, double delay, double sleep);
    void ledSetFlash(double onTime);
    
    void tick(void);
    uint32_t timeToTicks(double time);

    uint8_t state;

  typedef struct
  {
    uint32_t blinkPeriodTicks;
    uint32_t blinkOnTicks;
    uint32_t blinkCount;
  }ledBlinkParam_TypeDef;

  typedef struct
  {
    uint32_t onTicks;
  }ledFlashParam_TypeDef;

  typedef struct
  {
    uint32_t count;
    uint32_t currentCount;
    uint32_t countPauseTicks;
    uint32_t countOnTicks;
    uint32_t countOffTicks;  
  }ledCountParam_TypeDef;

  private:
    volatile uint32_t _ticks;
    unsigned long _last_millis;

    uint8_t _ledProcess();
    uint32_t _callsPerSecond;
    uint8_t _oldState;
    uint8_t _function;
    ledBlinkParam_TypeDef _blinkParam;
    ledCountParam_TypeDef _countParam;
    ledFlashParam_TypeDef _flashParam;
    
    void _ledFunctionStill();
    void _ledFunctionBlink();
    void _ledFunctionCount();
    void _ledFunctionFlash();
};

#endif