# Advanced topics

- [Performance](performance.md): latency and CPU load compared with hand-written register code, i.e.
  a minimal interrupt handler for a single timer channel; precomputed reload values for `triggerDirect()`, double callbacks, changing the CPU clock at runtime.
- [Avoiding PWM timer clashes](pwm-timer-clashes.md): which pins lose PWM when TeensyTimerTool uses a
  TMR or FTM module.
