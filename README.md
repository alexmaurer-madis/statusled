## Latest news

April 2023
* Less code and simpler way to use the library with the help of the class StatusLedManager

  

# Easy drive a status LED on your Arduino project

Every microcontroller project generally includes one or more status LED.  
This library helps you to easily display the current state of your device.

* LED still for on/off states.
* LED flashing for communication or storage information (TX/RX, READ/WRITE).
* LED blinking and counting to display precise states.

Flashing and blinking function can also be used with a buzzer ! Beep ! Beep-Beep-Beep...


## Easily set the LED blinking pattern
You can set the pattern of your LED on the fly by calling the following methods :

| Description           | Function                                                                |
| :-------------------- | :---------------------------------------------------------------------- |
| LED still (off or on) | `ledSetStill(uint8_t)`                                                  |
| LED blinking          | `ledSetBlink(double period, double dutyCycle)`                          |
| LED counting          | `ledSetCount(uint8_t count, double onTime, double delay, double sleep)` |
| LED flashing one time | `ledSetFlash(double onTime)`                                            |


## Example with the LED pattern "Counting"
With counting pattern and only one LED on your project allows you to display multiple states.

| LED flashing count | Example of state           |
| :----------------: | :------------------------- |
|         1x         | stand-by                   |
|         2x         | heating                    |
|         3x         | cooling                    |
|         4x         | temperature sensor failure |


```cpp
//Blink 4 times and wait 3s before blinking again
//Blinking pattern is 0.2s ON and 0.4s OFF
ledSetCount(4, 0.2, 0.4, 3);
```

# Library usage

## with loop() and millis() (the preferred way) :


```cpp
#include <statusled.h>

StatusLedManager slm;

void setup() {
  //It is up to you to set the correct output pin for your led
  pinMode(LED_BUILTIN, OUTPUT);

  //Create a new Status Led with a custom name
  slm.createStatusLed("ready", LED_BUILTIN);
  //Create more leds if needed...
  
  //Set the desired state by using the custom name you choose before to address the correct led
  slm("ready").ledSetBlink(2, 50);
}

void loop() {
  //Call the process method in your loop
  slm.process(millis());
}
```

## Old way, up to version 1.0.5 (you can still use it)

```cpp
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

Please see [Usage with a timer](Usage%20with%20a%20timer.md)

