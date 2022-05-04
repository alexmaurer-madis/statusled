# Drive a status LED on your project

Every project generally includes one or more status LED.  
This library helps you to easily display the state of your device.

Function "Counting" for example in a thermostat is very handy to indicate the current state like :

- 1 time, stand-by
- 2 times, heating
- 3 times, cooling
- 4 times, temperature sensor failure

## LED blinking pattern

You can set the pattern of your led on the fly within your code by calling the following functions :

| Description           | Function                                                               |
| :-------------------- | :--------------------------------------------------------------------- |
| LED still (off or on) | ledSetStill(uint8_t)                                                   |
| LED blinking          | ledSetBlink(double period, double dutyCycle)                           |
| LED counting          | ledSetCount(uint32_t count, double onTime, double delay, double sleep) |
| LED flashing one time | ledSetFlash(double onTime)                                             |

## Library usage with millis() and loop() (the preferred way) :

```
StatusLed sl = StatusLed();

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    //Blink a led for a period of 2s and a duty cycle of 50%
    sl.ledSetBlink(2, 50);

    //Blink 4 times and wait 3s before blinking again
    //sl.ledSetCount(4, 0.2, 0.4, 3);
}

void loop() {
  unsigned long current_millis=millis();

  //calling process return true if the state of the led must be changed.
  if(sl.process(current_millis)) {
    digitalWrite(LED_BUILTIN, sl.state);
  }
}
```

## Library usage with a timer (for ex. 1ms) and loop()

```
//1ms timer = 1000 ticks per second
StatusLed sl = StatusLed(1000);

//The initialisation of your timer and callback routine is specific to your platform
void timerRoutine(void) {
    sl.tick();
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    //Blink a led for a period of 2s and a duty cycle of 50%
    sl.ledSetBlink(2, 50);

    //Blink 4 times and wait 3s before blinking again
    //sl.ledSetCount(4, 0.2, 0.4, 3);
}

void loop() {

  //calling process return true if the state of the led must be changed.
  if(sl.process()) {
    digitalWrite(LED_BUILTIN, sl.state);
  }
}
```

## Todo list, library enhancement

- [ ] Reduce \_blinkParam, \_countParam, \_flashParam to a smaller footprint
