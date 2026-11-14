# CallbackWithParams

A periodic timer calls the function `callback(int& someInt, int cnt)` every 50 ms, which prints `someInt`
`cnt` times. `loop()` increments the variable passed as `someInt` once per second.

**What you learn**

- How to call a function that needs parameters from a timer: a lambda expression wraps the call,
  `t.begin([] { callback(number, 5); }, 50ms);`
- That a parameter passed by reference (`int&`) gives the callback the current value of the variable at
  every call.

**Boards:** all Teensy boards. **Hardware:** none, the output goes to the serial monitor.

**Output:** 20 lines per second; the number increases every second.

```
1 | 1 | 1 | 1 | 1 |
1 | 1 | 1 | 1 | 1 |
...
2 | 2 | 2 | 2 | 2 |
```

**Manual:** [Callbacks – Callbacks with parameters](https://luni64.github.io/TeensyTimerTool/callbacks/#callbacks-with-parameters)
