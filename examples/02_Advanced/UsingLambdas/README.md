# UsingLambdas

Every second, `loop()` switches the built-in LED on and triggers a one-shot timer, which switches it off
after 25 ms. The callback of the timer is a lambda expression, written directly in the call to `begin()`.

**What you learn**

- How to pass a lambda expression as callback: `timer.begin([] { digitalWriteFast(LED_BUILTIN, LOW); });`
  A separate function is not needed for such short callbacks.

**Boards:** all Teensy boards. **Hardware:** none, the sketch uses the built-in LED.

**Result:** the LED flashes briefly once per second.

**Manual:** [Callbacks – Lambda expressions](https://luni64.github.io/TeensyTimerTool/callbacks/#lambda-expressions)
