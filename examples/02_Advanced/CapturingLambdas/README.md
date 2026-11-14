# CapturingLambdas

Ten software timers with random periods between 100 and 900 ms share one callback function,
`onTimer(int nr)`. Each timer passes its own number to it, and the callback prints the number and the
time.

**What you learn**

- How a lambda expression *captures* a value: `[i] { onTimer(i); }` stores its own copy of the loop
  variable `i`, so every timer calls `onTimer()` with a different number.
- How to call a function with parameters from a callback.

**Boards:** all Teensy boards. **Hardware:** none, the output goes to the serial monitor.

**Output** (the numbers depend on the random periods):

```
Timer #9 at 433
Timer #6 at 443
Timer #3 at 459
Timer #1 at 525
Timer #0 at 532
...
```

**Manual:** [Callbacks – Capturing variables](https://luni64.github.io/TeensyTimerTool/callbacks/#capturing-variables)
