# Choosing a timer

!!! note "Work in progress"
    This chapter is being moved from the wiki and updated for v1.5.

!!! warning "Teensy 3.x: long periods"
    The FTM timers of Teensy 3 support periods of only about 35 … 45 ms. For longer periods, `begin()`
    returns the warning `periodOverflow` and the timer runs with its maximum period. Long periods
    require a software timer: `PeriodicTimer t1(TCK);`

## Resolution and rounding

Every timer counts ticks of its input clock, so internally a period is always an integer number of
ticks. The requested period is rounded to the nearest number of ticks, with a minimum of one tick.

Example: the TMR timers run on the 150 MHz peripheral clock of the Teensy 4, divided by the default
prescaler of 128. This gives a tick frequency of 150 MHz / 128 = 1.171875 MHz, i.e. a tick period of
0.853 µs. A requested period of 100 µs corresponds to 100 / 0.853 = 117.2 ticks; the timer uses
117 ticks = 99.84 µs. `getPeriod()` returns the period actually used, `getResolution()` the tick period.

The RTC timer supports only periods of 2⁻ⁿ seconds, i.e. 1 s, 0.5 s, 0.25 s, 125 ms, 62.5 ms, … down to
61 µs and 30.5 µs (2⁻¹⁵ s), and selects the next longer one (25 ms → 31.25 ms).
