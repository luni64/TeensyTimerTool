# Choosing a timer

!!! note "Work in progress"
    This chapter is being moved from the wiki and updated for v1.5.

!!! warning "Teensy 3.x: long periods"
    The FTM timers of Teensy 3 support periods of only about 35 … 45 ms. For longer periods, `begin()`
    returns the warning `periodOverflow` and the timer runs with its maximum period. Long periods
    require a software timer: `PeriodicTimer t1(TCK);`
