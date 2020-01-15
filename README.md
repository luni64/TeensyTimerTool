# TeensyTimerTool

TeensyTimerTool is a library providing an easy to use generic interface to the hardware timers of the PJRC Teensy boards.  In addition to the hardware timers it provides up to 20 highly efficient software timers with the same functionality. All timers can be used in periodic and one-shot mode. 
The library currently only supports T4.0 boards.

## Basic Usage

### Periodic Timer
The following sketch shows the basic usage of TeensyTimerTool. It allocates the next free timer from the pool of available timers and sets it up to call the callback function every 250ms. 

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

Timer t1; 

void setup()
{
    pinMode(LED_BUILTIN,OUTPUT);       
    t1.beginPeriodic(callback, 250'000); // 250ms       
}

void loop(){/*nothing*/}


void callback() // toggle the LED
{
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));    
}
```

The next example shows how to use a timer in one-shot mode by calling ```beginOneShot()``` instead of ```beginPeriodic()```. The timer can be started by the ```trigger()``` function which takes the delay time in microseconds. 

In ```loop()``` the LED is switched on every 500ms. After switching it on the one-shot timer is triggered to switch it off 10ms later. 

### One Shot Timer

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

Timer t1; 

void setup() 
{
    pinMode(LED_BUILTIN,OUTPUT);   
    t1.beginOneShot(callback); 
}

void loop()
{   
    digitalWriteFast(LED_BUILTIN, HIGH); 
    t1.trigger(10'000); // trigger the timer with 10ms delay

    delay(500);
} 

void callback() // switch off LED
{
    digitalWriteFast(LED_BUILTIN, LOW);  
}
```

## Supported Timers

### Teensy 4.0
Currently, the following timers are supported

#### General Purpose Timer (GPT)
The GPT is a 32bit timer with one timer channel per module. The controller of the T4.0 boards (IMXRT1060) implements two of those timer modules (GPT1 and GPT2)

*Usage*
```c++
// allocate the first (and only) channel of GPT1.
Timer t1(GPT1); 

//...
t.beginPeriodic(callback,1'00'000);
//...
```

#### QUAD Timer (TMR)
Teensy 4.0 has four Quad timer modules (TMR1 ... TMR4). Each of the modules has four 16bit timer channels. 
*Usage*
```c++

//...
Timer t1(TMR1);  // first free channel of TMR1
Timer t2(TMR1);  // next free channel of TMR1 (up to 4)
Timer t3(TMR3);  // first free channel of TMR3 (up to 4)

//...
```

#### Periodic Timer (PIT)
Not yet implemented

#### Tick Timer (TCK)
The tick timer is a 32bit software timer. Instead of using one of the built in hardware timers it relies on calling a *tick* function as often as possible. In that function it checks the cycle counter to determine if the callback function needs to be called. 
Calling that *tick* function is automatically handled by TeensyTimerTool in the ```yield()``` function (this will be optional later). So, in principle you can use the tick timer in the same way as the hardware timers. 

*Advantages:*
- Since it doesn't need any hardware resources, the number of timer channels is not limited in principle. However, to allow static memory allocation TeensyTimerTool limits the number of tick timers to 20 (that will be settable later)
- Due to the weak coupling of peripheral to the ARM core the hardware timers of the Teensy 4.0 are not very efficient. (e.g. [https://forum.pjrc.com/threads/57959-Teensy-4-IntervalTimer-Max-Speed](https://forum.pjrc.com/threads/57959-Teensy-4-IntervalTimer-Max-Speed?p=218577&viewfull=1#post218577))

*Caveats:*  
The tick timers only work if TeensyTimerTool can call the tick function as often as possible. This means if you have some long blocking tasks the tick timers might not work for you. Please note: delay() or other functions calling ```yield()``` in the background are perfectly fine to use. 

*Usage:*
```c++
// allocate one of the tick timers
Timer t(TCK); 
//...
t.beginPeriodic(callback,1'00'000);
//...
```

### Other Boards
To be implemented later

## Callback functions
### Traditional callbacks
tbd
### Using a Functor as callback
tbd
### Member Function Callbacks
tbd
### Templated Callbacks
tbd

## Configuration
tbd










