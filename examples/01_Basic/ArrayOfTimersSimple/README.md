# ArrayOfTimersSimple

Three software timers kept in an array. Each one has its own callback and period (1 s, 2 s, 3 s) and
prints a message to the serial monitor when it fires.

**What you learn**

- How to declare an array of timers and give each element its timer type: `PeriodicTimer t[]{TCK, TCK, TCK};`
- That the elements of the array are used like single timers: `t[0].begin(...)`.
- How to give periods with units (`1s`, `2s`), see the example UsingChronoDurations1.

**Boards:** all Teensy boards. **Hardware:** none, the output goes to the serial monitor.

**Output:**

```
 1396 ms: timer 0
 2396 ms: timer 0
 2396 ms: timer 1
 3396 ms: timer 0
 3396 ms: timer 2
 ...
```

The example ArrayOfTimers shows an array that mixes different timer types (Teensy 4).

**Manual:** [Usage – Arrays and containers of timers](https://luni64.github.io/TeensyTimerTool/usage/#arrays-and-containers-of-timers)
