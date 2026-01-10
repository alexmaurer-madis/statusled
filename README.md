## Latest news

### January 2026 - v1.2.0

* Added optional duration parameter to `ledSetBlink()` to automatically stop blinking after a specified time

* This revision significantly reduces RAM and ROM footprint, making the library more suitable for memory-constrained microcontrollers.

* Optional std::map: The StatusLedManager no longer requires std::map by default. Instead, it uses a fixed-size array for LED storage, reducing compiled size by approximately 90%.

* Tested with c++11 and c++17

### December 2024 - v1.1.1
* Code rework on class StatusLedManager

# Easy drive a status LED on your Arduino project

Every microcontroller project generally includes one or more status LED.  
This library helps you to easily display the current state of your device.

* LED still for on/off states.
* LED flashing for communication or storage information (TX/RX, READ/WRITE).
* LED blinking and counting to display precise states.

Flashing and counting function can also be used with a buzzer ! Beep ! Beep-Beep-Beep...


## Easily set the LED blinking pattern
You can set the pattern of your LED on the fly by calling the following methods :

| Description           | Function                                                                |
| :-------------------- | :---------------------------------------------------------------------- |
| LED still (off or on) | `ledSetStill(uint8_t)`                                                  |
| LED blinking          | `ledSetBlink(double period, double dutyCycle, uint32_t duration_ms=0)`  |
| LED counting          | `ledSetCount(uint8_t count, double onTime, double delay, double sleep)` |
| LED flashing one time | `ledSetFlash(double onTime)`                                            |

The `duration_ms` parameter is optional. Set to `0` (default) for indefinite blinking, or specify a duration in milliseconds to automatically stop after that time (max 3600000 = 1 hour).


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

## Example with the LED pattern "Blinking" with duration

```cpp
//Blink indefinitely with period 0.5s and 50% duty cycle
ledSetBlink(0.5, 50);

//Blink for 20 seconds then stop automatically
ledSetBlink(0.5, 50, 20000);
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

# Configuration option

Define these macros before including the header:

```cpp
// Use a fixed-size array (default: 4 LEDs max)
#define STATUSLED_MAX_LEDS 2    // Adjust to your needs
#include "statusled.h"
```

```cpp
// Or use std::map for unlimited LEDs (larger footprint)
#define STATUSLED_USE_MAP
#include "statusled.h"
```

Note: When using array mode, createStatusLed() silently ignores requests beyond STATUSLED_MAX_LEDS. Ensure this value matches your project requirements.

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

