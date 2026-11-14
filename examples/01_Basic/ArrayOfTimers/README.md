# ArrayOfTimers

An array of five one-shot timers of different types generates pulses of different lengths on pins 0 … 4.
Every millisecond, `loop()` sets all five pins HIGH and triggers the timers; each callback sets its pin
LOW again after 50, 100, 150, 200 and 250 µs.

**What you learn**

- How to declare an array of timers and give each element its own timer type. The types can be mixed:
  `OneShotTimer t[]{GPT1, TMR1, TMR1, TCK, TCK};` uses the GPT1 timer, two channels of the TMR1 module and
  two software timers.
- How to set up all elements in a loop, with a lambda expression that captures the index `i` as callback.

**Boards:** Teensy 4.0, 4.1, MicroMod (GPT and TMR timers). For other boards, use the commented line
`OneShotTimer t[5];`, which takes the timers from the timer pool, or see ArrayOfTimersSimple.

**Hardware:** an oscilloscope or logic analyzer on pins 0 … 4 shows the pulses.

**Manual:** [Usage – Arrays and containers of timers](https://luni64.github.io/TeensyTimerTool/usage/#arrays-and-containers-of-timers),
[Choosing a timer](https://luni64.github.io/TeensyTimerTool/choosing-a-timer/)
