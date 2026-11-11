# Performance

TeensyTimerTool is designed to add as little overhead as possible to the timer hardware. Its interrupt
handlers and `trigger()` functions avoid reading timer registers, which are slow to access on the
Teensy 4 (about 100 CPU cycles each for GPT and PIT), and use the same register writes a hand-written
implementation would use.

## Comparison with hand-written code

The following values were measured on a Teensy 4.1 at 600 MHz, compared to hand-written register code
doing the same job in the same firmware (reference values in brackets):

| Timer | Latency (timer event → callback) | CPU time per interrupt | Duration of `trigger()` |
|---|---|---|---|
| GPT | 160 ns (153 ns) | 126 cycles (124) | 290 ns (345 ns) |
| PIT | 220 ns (153 ns) | 226 cycles (127) | 220 ns (148 ns) |
| TMR | 140 ns (113 ns) | 88 cycles (68) | 150 ns (120 ns) |

The PIT channels share one interrupt; the library reads one status register to find the channel that
fired, which accounts for the difference to the reference.

## Callback latency

A hardware timer signals the end of a period with an interrupt. Before the callback starts, the processor
enters the interrupt handler, and the library's handler identifies the timer channel, resets its
interrupt flag and calls the callback. This takes the latencies listed above, only slightly more than a
hand-written interrupt handler needs.

The software timers (TCK) don't use interrupts. They are evaluated in `yield()`, which runs after each
`loop()` iteration and during `delay()`. Their callbacks are therefore delayed by a few µs, and by
considerably more if `loop()` blocks for a long time.

<!-- TODO: measurement method, one-shot delay trigger -> callback, double callbacks / DSB, CPU clock changes -->
