# TeensyTimerTool

TeensyTimerTool is a library, providing an easy to use generic interface to the hardware timers of the PJRC Teensy boards.  Additionally it provides up to 20 highly efficient software timers with the same interface. All timers can be used in periodic and one-shot mode. 
The library currently only supports T4.0 boards.
You can either choose the next free timer from a pool or specify exactly which hard- or software timer module you want to use. 

## Basic Usage

### Periodic Timer
The following sketch shows the basic usage of TeensyTimerTool. It allocates the next free timer from a pool of available timers and sets it up to periodically call the callback function every 250ms. 

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

### One Shot Timer
You can use the timer in one-shot mode by calling ```beginOneShot()``` instead of ```beginPeriodic()```. A one shot timer can be started by the ```trigger(unsigned delay)``` function which expects the delay time in microseconds. In ```loop()``` the LED is switched on every 500ms. After switching it on the one-shot timer is triggered to switch it off 10ms later. 


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

The following table shows available timers for the Teensy boards. Entries show m x c where m is the number of modules and c is the number of channels per module. Modules with entries in parentheses are not yet implemented. Modules for entries with dashes are not available for the board. Details can be found in the corresponding processor [datasheets](https://www.pjrc.com/teensy/datasheets.html)

|Timer Module|                      |Width| T4.0 |T3.6  |T3.5  |T3.2  |T-LC  |
|---         | -                    | :-: | :-:  |:-:   |:-:   |:-:   |:-:   |
|TMR1...TMR4 |QUAD Timer            |16bit| 4x4  |-     | -    | -    | -    |
|GPT1...GPT2 |General Purpose Timer |32bit| 2x1  |  -   | -    | -    | -    |
|PIT1...PITn |Periodic Timer        |32bit|(4x1) |(4x1) |(4x1) |(4x1) |(2x1) |
|FTM1...FTMn |Flex Timer            |16bit| -    |  ?   |   ?  | ?    | ?    |
|TCK         |Tick-Timer            |32bit| 1x20 |(1x20)|(1x20)|(1x20)|(1x20)|



### General Purpose Timer (GPT)
The general purpose timer module (GPT) is a 32bit timer module with one timer channel. The controller of the T4.0 boards (IMXRT1062) implements two of those timer modules (GPT1 and GPT2). Currently the GPT modules are not used by the Teensyduino core libraries.

```c++
// allocate two timers using GPT1 and GPT2 respectively
Timer t1(GPT1); 
Timer t2(GPT2);
```

### QUAD Timer (TMR)
The QUAD modules (TMR) are 16 bit timer modules with 4 timer channels per module. 
Teensy 4.0 controller has four TMR modules (TMR1 ... TMR4). The Teensyduino core uses TMR1-TMR3 for generating PWM signals. Using one of those will disable the PMW capability of the corresponding pins. 

```c++
Timer t1(TMR1);  // first free channel of TMR1
Timer t2(TMR1);  // next free channel of TMR1 (up to 4)
Timer t3(TMR3);  // first free channel of TMR3 
...
```

### Periodic Timer (PIT)
Not yet implemented

### Tick Timer (TCK)
The tick timer is a 32bit software timer. Instead of using one of the built in hardware timer modules it relies on calling a *tick* function as often as possible.  That function checks the cycle counter to determine if the callback function needs to be called. 
Calling the *tick* function is automatically handled by TeensyTimerTool in the ```yield()``` function (this will be optional later). Thus, you can use the tick timer in exactly the same way as the hardware timers. 
```c++
Timer t1(TCK), t2(TCK), t3(TCK); // three tick timers

void setup()
{
    t1.beginPeriodic(callback1,200);
    t2.beginPeriodic(callback2,20'000'000);
    t3.beginOneShot(callback3);
}
```

**Advantages:**
- Since the tick timers don't need any hardware resources, you can allocate as many timers you need. However, to allow static memory allocation TeensyTimerTool currently limits the number of tick timers to 20 (that will be settable later).
- Due to the weak coupling of peripheral to the ARM core the hardware timers of the Teensy 4.0 are not very efficient. (e.g. [https://forum.pjrc.com/threads/57959-Teensy-4-IntervalTimer-Max-Speed](https://forum.pjrc.com/threads/57959-Teensy-4-IntervalTimer-Max-Speed?p=218577&viewfull=1#post218577)). Given the very fast ARM core of the T4.0 controller the tick timers can be more effective and faster than the hardware timers on this board.

**Caveats:**   
- The tick timers only work if TeensyTimerTool can call the *tick* function with a high frequency. This means if you can not avoid long blocking tasks the tick timers might not work for you. (Please note: ```delay()``` or other functions calling ```yield()``` in the background are perfectly fine to use.)



## Callback functions
By default, TeensyTimerTool uses callbacks of type std::function. This allows the user to attach pretty much all callable objects to a timer. Callable objects include e.g.
- Traditional callbacks, i.e. pointers to void functions
- Funktors as callback objects
- Static and non static member functions
- Lambdas

In case you prefer a plain vanilla function pointer interface you can configure TeensyTimerTool accordingly.

### Traditional callbacks
As usual you can simply attach a pointer to a parameter less void function. 
```c++
Timer t1; 

void plainOldCallback()
{ 
  // do something 
}

void setup()
{    
    t1.beginPeriodic(plainOldCallback, 1000);
}
```

### Using Funktors as callback
[Funktors](https://stackoverflow.com/questions/356950/what-are-c-functors-and-their-uses) are classes with an overridden function call operator and can be used as callback objects. The overridden *operator()* will be used as callback. The following example shows a funktor which generates a pulse with adjustable pulse width. 
```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

class PulseGenerator
{
public:
    PulseGenerator(unsigned _pin, unsigned _pulseLength)
        : pin(_pin), pulseLength(_pulseLength)
    {        
    }

    inline void operator()()
    {
        digitalWriteFast(pin, HIGH);
        delayMicroseconds(pulseLength);
        digitalWriteFast(pin, LOW);
    }

protected:
    unsigned pin, pulseLength;
};

//==============================================================

Timer t1, t2;

void setup()
{
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);

    t1.beginOneShot(PulseGenerator(1, 5));  // 5µs pulse on pin 1
    t2.beginOneShot(PulseGenerator(2, 10)); //10µs pulse on pin 2
}

void loop()
{    
    t1.trigger(1'000); 
    t2.trigger(500);
    delay(10);
}
```

### Timer Embedded in Class
You can also define a class which embeds one or more timers and use a non static member function as callback. The following example shows a *Blinker* class which uses a timer to periodically toggles a pin in the background. The somehow ugly syntax of attaching the member function as callback is explained [here](https://stackoverflow.com/a/7582576/1842762)

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

class Blinker
{
public:
    Blinker(unsigned pin, unsigned period)
    {
        this->pin = pin;
        this->period = period;        
    }

    void begin()
    {
        pinMode(pin,OUTPUT);
        timer.beginPeriodic(std::bind(&Blinker::blink, this), period);
    }

    void blink() // callback function
    {
        digitalWriteFast(pin, !digitalReadFast(pin)); // toggle pin
    }

protected:
    Timer timer;  
    unsigned pin, period;
};

//==============================================================

Blinker b1(1, 100'000);  // blinks on pin 1 with period 100ms
Blinker b2(2, 50'000);   // blinks on pin 2 with period  50ms

void setup()
{
    b1.begin();
    b2.begin();   
}

void loop()
{   
}
```


## Configuration
tbd

