# Drive one or more status LED on your Arduino project

Every Arduino project generally includes one or more status LED.  
This library helps you to easily display the current state of your device.

## Example with the LED pattern "Counting"

Only one LED on a small thermostat can be used to indicate :

| LED flashing count | State description          |
| :----------------: | :------------------------- |
|         1x         | stand-by                   |
|         2x         | heating                    |
|         3x         | cooling                    |
|         4x         | temperature sensor failure |

## Easily set the LED blinking pattern

You can set the pattern of your LED on the fly within your code by calling the following functions :

| Description           | Function                                                               |
| :-------------------- | :--------------------------------------------------------------------- |
| LED still (off or on) | ledSetStill(uint8_t)                                                   |
| LED blinking          | ledSetBlink(double period, double dutyCycle)                           |
| LED counting          | ledSetCount(uint32_t count, double onTime, double delay, double sleep) |
| LED flashing one time | ledSetFlash(double onTime)                                             |

**Example**

```
//Blink 4 times and wait 3s before blinking again
//Blinking pattern is 0.2s ON and 0.4s OFF
sl.ledSetCount(4, 0.2, 0.4, 3);
```

## Library usage with millis() and loop() (the preferred way) :

```
#include <statusled.h>

StatusLed sl = StatusLed();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Continuously blink the LED. Blink period is 2s and duty cycle 50% (1s on, 1s off).
  sl.ledSetBlink(2, 50);
}

void loop() {
  //Saving millis result here to remove multiple call overhead (if you have multiple LED to update)
  unsigned long current_millis=millis();

  //Calling process will return true if the state of the LED has changed.
  if(sl.process(current_millis)) {

    //Update output (use !sl.state if you must invert the output)
    digitalWrite(LED_BUILTIN, sl.state);
  }
}
```

## Library usage with a timer (for ex. interruption every 1ms) and loop()

```
#include <statusled.h>

//1ms timer = 1000 ticks per second
StatusLed sl = StatusLed(1000);

//The initialization of your timer and the ISR routine is specific to your board.
//Following function is only for usage demonstration
void timerRoutine(void) {
  sl.tick();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Continuously blink the LED. Blink period is 2s and duty cycle 50% (1s on, 1s off).
  sl.ledSetBlink(2, 50);
}

void loop() {

  //Calling process will return true if the state of the LED has changed.
  if(sl.process()) {

    //Update output (use !sl.state if you must invert the output)
    digitalWrite(LED_BUILTIN, sl.state);
  }
}
```

## Todo list for maintainer, library enhancement

- [ ] Reduce \_blinkParam, \_countParam, \_flashParam for a smaller footprint
