# Library usage with a timer (for ex. interruption every 1ms) and loop()

## Up to version 1.0.5 (you can still use it)

```cpp
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