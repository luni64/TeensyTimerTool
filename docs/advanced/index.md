# Advanced topics

!!! note "Work in progress"
    This chapter is being moved from the wiki and updated for v1.5.

## Efficiency

TeensyTimerTool is designed to add as little overhead as possible to the timer hardware. Its interrupt
handlers and `trigger()` functions avoid reading timer registers, which are slow to access on the
Teensy 4 (about 100 CPU cycles each for GPT and PIT), and use the same register writes a hand-written
implementation would use.

The following values were measured on a Teensy 4.1 at 600 MHz, compared to hand-written register code
doing the same job in the same firmware (reference values in brackets):

| Timer | Latency (timer event → callback) | CPU time per interrupt | Duration of `trigger()` |
|---|---|---|---|
| GPT | 160 ns (153 ns) | 126 cycles (124) | 290 ns (345 ns) |
| PIT | 220 ns (153 ns) | 226 cycles (127) | 220 ns (148 ns) |
| TMR | 140 ns (113 ns) | 88 cycles (68) | 150 ns (120 ns) |

The PIT channels share one interrupt; the library reads one status register to find the channel that
fired, which accounts for the difference to the reference.

<!-- TODO: methodology, triggerDirect(), double callbacks / DSB, changing the CPU clock at runtime -->
