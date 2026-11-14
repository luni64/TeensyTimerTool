# PinInformation

Prints, for every digital pin of the board, its GPIO register and the hardware timer that generates its
PWM signal. The list is printed three times: sorted by pin number, by PWM timer and by GPIO register.

**What you learn**

- Which pins lose their PWM signal when TeensyTimerTool uses a TMR (Teensy 4) or FTM (Teensy 3) module: a
  timer module either generates PWM or serves as TeensyTimerTool timer.

The sketch doesn't use TeensyTimerTool itself.

**Boards:** all Teensy boards. **Hardware:** none, the output goes to the serial monitor.

**Output** (excerpt, Teensy 4.1):

```
Pin |  GPIO Reg  |  PWM timer
----|------------|-------------
00  |  GPIO6_03  |  FLEX_PWM1
01  |  GPIO6_02  |  FLEX_PWM1
02  |  GPIO9_04  |  FLEX_PWM4
...
10  |  GPIO7_00  |  QUAD1
11  |  GPIO7_02  |  QUAD1
...
```

`QUAD1` is the TMR1 module: its pins (10, 11 and 12 on the Teensy 4.1) lose their PWM signal when TeensyTimerTool
uses TMR1.

**Manual:** [Avoiding PWM timer clashes](https://luni64.github.io/TeensyTimerTool/advanced/pwm-timer-clashes/)
