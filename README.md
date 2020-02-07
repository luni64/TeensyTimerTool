# TeensyTimerTool

TeensyTimerTool is a library that provides a generic, easy to use interface to the hardware timers of the PJRC Teensy boards.  In addition, it provides up to 20 highly efficient software timers that use the same interface. All timers can be used in periodic and one-shot mode. Currently the library supports the ARM T3.X and T4.0 boards.
You can either pick a free timer from a pool or specify exactly which of the available hardware or software timer modules you want to use.

See here https://github.com/luni64/TeensyTimerTool/edit/master/README.md for the corresponding thread in the PJRC forum.

<!-- vscode-markdown-toc -->
* [Basic Usage](#BasicUsage)
	* [Periodic Timer](#PeriodicTimer)
	* [One-Shot Timer](#One-ShotTimer)
* [Supported Timers](#SupportedTimers)
	* [GPT - General Purpose Timer](#GPT-GeneralPurposeTimer)
	* [TMR aka QUAD Timer](#TMRakaQUADTimer)
	* [PIT - Periodic Timer](#PIT-PeriodicTimer)
	* [TCK - Tick Timer](#TCK-TickTimer)
* [Callback Functions](#CallbackFunctions)
	* [Traditional Callbacks](#TraditionalCallbacks)
	* [Functors as Callback Objects](#FunctorsasCallbackObjects)
	* [Lambda Expressions and Callbacks with Context](#LambdaExpressionsandCallbackswithContext)
	* [How to Encapsulate a Timer and its Callback in a Class](#HowtoEncapsulateaTimeranditsCallbackinaClass)
* [Error Handling](#ErrorHandling)
	* [Manually Check for Errors](#ManuallyCheckforErrors)
	* [Using an Error Handling Callback](#UsinganErrorHandlingCallback)
	* [Using the Built in Error Handler](#UsingtheBuiltinErrorHandler)
* [Configuration](#Configuration)

<!-- vscode-markdown-toc-config
	numbering=false
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->


## <a name='BasicUsage'></a>Basic Usage

### <a name='PeriodicTimer'></a>Periodic Timer
The following sketch shows the basic use of the TeensyTimerTool. It picks the next free timer from a pool of available timers and sets it up to call the callback function every 250ms.

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1;

void setup()
{
    pinMode(LED_BUILTIN,OUTPUT);
    t1.begin(callback, 250'000); // 250ms
}

void loop(){/*nothing*/}


void callback() // toggle the LED
{
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}
```

### <a name='One-ShotTimer'></a>One-Shot Timer
You can also use one-shot timers. Instead of invoking the callback periodically it is called only once after starting the timer with the ``trigger`` function.

The example below defines a one-shot timer whose callback simply switches off the board LED. In ``loop()``  the LED is switched on every 500ms. Immediately afterwards the timer is triggered with a delay time of 10ms so that the LED flashes briefly every 500ms.


```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer t1;

void setup()
{
    pinMode(LED_BUILTIN,OUTPUT);
    t1.begin(callback);
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

## <a name='SupportedTimers'></a>Supported Timers

The following table shows available timers for the Teensy boards. Entries show m x c where m is the number of modules and c is the number of channels per module. Modules with entries in parentheses are not yet implemented. Modules for entries with dashes are not available for the board. Details can be found in the corresponding processor [datasheets](https://www.pjrc.com/teensy/datasheets.html)

|Timer Module|                      |Width| T4.0 |T3.6  |T3.5  |T3.2  |T-LC  |
|---         | -                    | :-: | :-:  |:-:   |:-:   |:-:   |:-:   |
|TMR1...TMR4 |QUAD Timer            |16bit| 4x4  |-     | -    | -    | -    |
|GPT1...GPT2 |General Purpose Timer |32bit| 2x1  |  -   | -    | -    | -    |
|PIT1...PITn |Periodic Timer        |32bit|(4x1) |(4x1) |(4x1) |(4x1) |(2x1) |
|FTM1...FTMn |Flex Timer            |16bit| -    |  20  |  20  | 12   |  -   |
|TCK         |Tick-Timer            |32bit| 1x20 | 1x20 | 1x20 | 1x20 | 1x20 |



### <a name='GPT-GeneralPurposeTimer'></a>GPT - General Purpose Timer
The general purpose timer module (GPT) is a 32bit timer module with one timer channel. The controller of the T4.0 boards (IMXRT1062) implements two of those timer modules (GPT1 and GPT2). Currently the GPT modules are not used by the Teensyduino core libraries.

```c++
// allocate two timers using GPT1 and GPT2 respectively
PeriodicTimer t1(GPT1);
PeriodicTimer t2(GPT2);
```

### <a name='TMRakaQUADTimer'></a>TMR aka QUAD Timer
The QUAD modules (TMR) are 16 bit timer modules with 4 timer channels per module.
Teensy 4.0 controller has four TMR modules (TMR1 ... TMR4). The Teensyduino core uses TMR1-TMR3 for generating PWM signals. Using one of those will disable the PMW capability of the corresponding pins.

```c++
PeriodicTimer t1(TMR1); // first free channel of TMR1
OneShotTimer  t2(TMR1); // next free channel of TMR1 (up to 4)
PeriodicTimer t3(TMR3); // first free channel of TMR3
...
```

### <a name='PIT-PeriodicTimer'></a>PIT - Periodic Timer
Not yet implemented

### <a name='TCK-TickTimer'></a>TCK - Tick Timer
The tick timer is a 32bit software timer. Instead of using one of the built in hardware timer modules it relies on calling a *tick* function as often as possible.  That function checks the cycle counter to determine if the callback function needs to be called.
Calling the *tick* function is automatically handled by TeensyTimerTool in the ```yield()``` function (this will be optional later). Thus, you can use the tick timer in exactly the same way as the hardware timers.
```c++
PeriodicTimer t1(TCK), t2(TCK);
OneShotTimer  t3(TCK);

void setup()
{
    t1.begin(callback1,200);
    t2.begin(callback2,20'000'000);
    t3.begin(callback3);
}
```

**Advantages:**
- Since the tick timers don't need any hardware resources, you can allocate as many timers you need. However, to allow static memory allocation TeensyTimerTool currently limits the number of tick timers to 20 (that will be settable later).
- Due to the weak coupling of peripheral to the ARM core the hardware timers of the Teensy 4.0 are not very efficient. (e.g. [https://forum.pjrc.com/threads/57959-Teensy-4-IntervalTimer-Max-Speed](https://forum.pjrc.com/threads/57959-Teensy-4-IntervalTimer-Max-Speed?p=218577&viewfull=1#post218577)). Given the very fast ARM core of the T4.0 controller the tick timers can be more effective and faster than the hardware timers on this board.

**Caveats:**
- The tick timers only work if TeensyTimerTool can call the *tick* function with a high frequency. This means if you can not avoid long blocking tasks the tick timers might not work for you. (Please note: ```delay()``` or other functions calling ```yield()``` in the background are perfectly fine to use.)



## <a name='CallbackFunctions'></a>Callback Functions
By default, TeensyTimerTool uses callbacks of type std::function. This allows the user to attach pretty much all callable objects to a timer. Callable objects include e.g.
- Traditional callbacks, i.e. pointers to void functions
- Functors as callback objects
- Static and non static member functions
- Lambdas

In case you prefer a plain vanilla function pointer interface you can configure TeensyTimerTool accordingly.

### <a name='TraditionalCallbacks'></a>Traditional Callbacks
As usual you can simply attach a pointer to a parameter less void function.
```c++
PeriodicTimer t1;

void plainOldCallback()
{
  // do something
}

void setup()
{
    t1.begin(plainOldCallback, 1000);
}
```

### <a name='FunctorsasCallbackObjects'></a>Functors as Callback Objects
[Functors](https://stackoverflow.com/questions/356950/what-are-c-functors-and-their-uses) are classes with an overridden function call operator and can be used as callback objects. The overridden *operator()* will be used as callback. The following example shows a functor which generates a pulse with adjustable pulse width.
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

OneShotTimer t1, t2;

void setup()
{
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);

    t1.begin(PulseGenerator(1, 5));  // 5µs pulse on pin 1
    t2.begin(PulseGenerator(2, 10)); //10µs pulse on pin 2
}

void loop()
{
    t1.trigger(1'000);
    t2.trigger(500);
    delay(10);
}
```

### <a name='LambdaExpressionsandCallbackswithContext'></a>Lambda Expressions and Callbacks with Context

Using lambda expressions as callbacks allows for some interesting use cases. If you are not familiar with lambdas, here a nice write up from Sandor Dago http://sandordargo.com/blog/2018/12/19/c++-lambda-expressions.

Let's start with a simple example:

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer t1;

void setup()
{
    while(!Serial);

    t1.begin([] { Serial.printf("I'm called at %d ms\n", millis()); });

    Serial.printf("Triggered at  %d ms\n", millis());
    t1.trigger(100'000); // 100ms
}

void loop(){}
```
Output:
```
Triggered at  384 ms
I'm called at 484 ms
```

The example shows that there is no need to create a dedicated callback function. You can directly define it as a lambda expression in the call to beginOneShot. That's nice, but not very exciting.

It gets more interesting when you need to pass context to a callback function:

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

elapsedMillis stopwatch;
OneShotTimer t1, t2, t3;

void callbackWithContext(unsigned pin)
{
    int callTime = stopwatch;
    Serial.printf("Do something with pin %u (%u ms)\n", pin, callTime);
}

void setup()
{
    while(!Serial);

     t1.begin([] { callbackWithContext(1); });
     t2.begin([] { callbackWithContext(2); });
     t3.begin([] { callbackWithContext(3); });

     stopwatch = 0;

     t1.trigger(20'000);
     t2.trigger(5'000);
     t3.trigger(50'000);
}

void loop()
{
}
```
Output:
```
Do something with pin 2 (5 ms)
Do something with pin 1 (20 ms)
Do something with pin 3 (50 ms)
```
It would be quite tedious to achieve the same with traditional function pointer callbacks.

A interesting use of this pattern is to call non static member functions. To demonstrate this let's assume you want to write a class which is supposed to blink on a specific pin. It takes the pin number in the constructor and toggles the pin whenever you call its blink function.
```c++
class Blinker
{
 public:
    Blinker(unsigned _pin)
    {
        pin = _pin;
        pinMode(pin, OUTPUT);
    }

    void blink() const
    {
        digitalWriteFast(pin, !digitalReadFast(pin));
    }

    protected:
       unsigned pin;
};
```

Then, simply use a lambda expression to call the blink member functions from the lambda callback of a timer object.

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

// copy Blinker class definition here or use separate .h file

PeriodicTimer t1, t2, t3;

Blinker b1(1);           // blinks on pin 1
Blinker b2(7);           // blinks on pin 7
Blinker b3(LED_BUILTIN); // blinks the built in LED

void setup()
{
    t1.begin([] { b1.blink(); }, 1'000);
    t2.begin([] { b2.blink(); }, 2'000);
    t3.begin([] { b3.blink(); }, 50'000);
}

void loop() {}
```

### <a name='HowtoEncapsulateaTimeranditsCallbackinaClass'></a>How to Encapsulate a Timer and its Callback in a Class

The Blinker class from the previous example can be improved further. Since the timers are only needed to call the blink functions it would be much more elegant to directly encapsulate them in the class. Then, the timers could be seen as an implementation detail of the Blinker class and don't need to be known outside the class at all. The same applies for the blinker function which can be hidden (protected / private) as well.

So, lets try to get rid of the globally defined timers and their initialization in setup().
First move the blink function to the protected part of the class and add the timer as a member variable. The interesting part happens in beginPeriodic().
 As callback to the timer we define a lambda expression which captures the *this* pointer and uses it to call our own blink() member function.

Here the complete code:

File *blinker.h*
```c++
#pragma once

class Blinker
{
 public:
    Blinker(unsigned _pin, unsigned _period) // add blink period to the constructor
    {
        pin = _pin;
        period = _period;
    }

    void begin()  // better not initalizie peripherals in constructors
    {
        pinMode(pin, OUTPUT);
        timer.begin([this] { this->blink(); }, period);
    }

 protected:
    void blink() const // this will be called by the timer
    {
        digitalWriteFast(pin, !digitalReadFast(pin));
    }

    unsigned pin, period;
    PeriodicTimer timer;
};
```

File *someSketch.ino*
```c++
#include "blinker.h"

Blinker b1(1, 1'000);            // blinks on pin 1, 1ms
Blinker b2(7, 2'000);            // blinks on pin 7, 2ms
Blinker b3(LED_BUILTIN, 50'000); // blinks the built in LED, 50ms

void setup()
{
    b1.begin();
    b2.begin();
    b3.begin();
}

void loop() {}
```

The new Blinker class has a very clean public interface. It completely encapsulates the timer and its callback which makes using the class much easier. The user code can simply define some Blinker objects and and doesn't need to know anything about timers, lambdas and other nerdy stuff.


## <a name='ErrorHandling'></a>Error Handling

Things can and will go wrong in programming. E.g. assume that you try to acquire three channels of  an FTM1 module which only provides two channels

```c++
OneShotTimer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    t1.begin(callback1);
    t2.begin(callback2);
    t3.begin(callback3);
}
...
```
This will lead to a runtime error when you try to initialize t3 in the last line. TeensyTimerTool won't crash on it but t3 will never work of course.

### <a name='ManuallyCheckforErrors'></a>Manually Check for Errors

So, it is a good idea to check if the acquisition of a timer didn't produce an error. Here an example showing how this can be done. In case of an error a ```panic``` function will be called which simply prints out the error and enters an endless, fast blink loop to signal a problem.

```c++
OneShotTimer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    ...
    errorCode err;
    err = t1.begin(callback1);
    if (err != errorCode::OK)
    {
        panic(err);
    }

    err = t2.begin(callback2);
    if (err != errorCode::OK)
    {
        panic(err);
    }
    ...
}


void panic(errorCode err)  // print out error code, stop everything and do a fast blink
{
    Serial.printf("Error %d\n", err);
    while(1)
    {
        digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
        delay(50);
    }
}
...
```
All defined error codes can be found in the file 'ErrorHandling/error_codes.h'

### <a name='UsinganErrorHandlingCallback'></a>Using an Error Handling Callback

Although the  example above works perfectly, it is quite tedious to always check things for possible errors. To make error handling more convenient you can attach an error handler function to the library. You first need to initialize it by calling  ```attachErrFunc``` as shown in the example below. Now, TeensyTimerTool automatically calls the attached error function whenever something goes wrong.

```c++
OneShotTimer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    Timer::attachErrFunc(panic);

    while(!Serial);
    pinMode(LED_BUILTIN, OUTPUT);

    t1.begin(callback1);
    t2.begin(callback2);
    t3.begin(callback3);
}


void panic(errorCode err)  // print out error code, stop everything and do a fast blink
{
    Serial.printf("Error %d\n", err);
    while(1)
    {
        digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
        delay(50);
    }
}
...
```

### <a name='UsingtheBuiltinErrorHandler'></a>Using the Built in Error Handler
TeensyTimerTool provides a standard error handler which prints out the error number and the corresponding error message. The constructor of the error handler expects a Stream reference (e.g. Serial or Serial1...) on which the messages are printed.

- In case of a **warning** the information is printed  and the error handler returns control to the calling code.

- In case of an **error** the error handler prints the message and enters a endless 50ms blink loop on LED_BUILTIN.

**WARNING:**
The constructor of the error handler sets the pin mode of LED_BUILTIN to OUTPUT and blinks on that pin in case of an error. **Do not use the standard error handler if you have something connected on pin LED_BUILTIN.**


Here a quick example showing how to use the built in standard error handler for printing on Serial.

```c++
OneShotTimer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    OneShotTimer::attachErrFunc(ErrorHandler(Serial));

    while(!Serial);
    pinMode(LED_BUILTIN, OUTPUT);

    t1.beginOneShot(callback1);
    t2.beginOneShot(callback2);
    t3.beginOneShot(callback3);
}
...
```

Output:
```
Error: 104: Timer module has no free channel
```

## <a name='Configuration'></a>Configuration
TeensyTimerTool can be configured in the following two ways.

**Change Settings Globally**

To change settings for all sketches you can edit the file ./src/defautConfig.h in the library folder. Changing this file affects all sketches using TeensyTimerTool. Of course, updates of the library or reinstallation will overwrite the changes. So, if you want to go that route make sure to do a backup of the config file before updating.

**Project Scope Settings**

Instead of changing global settings in defaultConfig.h you can also copy the file to your sketch folder and rename it to 'userConfig.h'. If TeensyTimerTool finds this file it will read it instead of the default config.

*Note: Depending on your build system, copying the config file might require a clean rebuild. This of course is only necessary once after copying the file. Subsequent config changes will be detected as usual and do not require a clean rebuild.*

**Settings**

The following block shows the available settings.
```c++
#pragma once

#include "boardDef.h"
namespace TeensyTimerTool
{
//--------------------------------------------------------------------------------------------
// Timer pool defintion
// Add and sort as required

#if defined(T4_0)
    TimerGenerator* const timerPool[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};

#elif defined(T3_6)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, FTM2, FTM3, FTM4, TCK};

#elif defined(T3_5)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, FTM2, FTM3, TCK};

#elif defined(T3_2)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, FTM2, TCK};

#elif defined(T3_0)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, TCK};

#elif defined(TLC)
    TimerGenerator* const timerPool[] = {TCK};

#elif defined(ESP32)
    TimerGenerator* const timerPool[] = {TCK};

#elif defined(UNO)
    TimerGenerator* const timerPool[] = {TCK};
#endif
    constexpr unsigned timerCnt = sizeof(timerPool) / sizeof(timerPool[0]);

//--------------------------------------------------------------------------------------------
// Timer Settings
//
// Default settings for various timers

// TMR (QUAD)
    constexpr unsigned TMR_DEFAULT_PSC = 7;  // default prescaler, 0..7 -> prescaler= 1,2,4,...128, timer clock f=150MHz

// GPT & PID
    constexpr bool USE_GPT_PIT_150MHz = false;// changes the clock source for GPT and PIT from 24MHz (standard) to 150MHz, might have side effects!


//--------------------------------------------------------------------------------------------
// Callback type
// Uncomment if you prefer function pointer callbacks instead of std::function callbacks

//    #define PLAIN_VANILLA_CALLBACKS


//--------------------------------------------------------------------------------------------
// Advanced Features
// Uncomment if you need access to advanced features

//#define ENABLE_ADVANCED_FEATURES
}
```
**TimerPool Settings**   \
The `TimerPool` settings define which timer modules should be available to the timer pool. If you look at the setting for T4.0 you see that the pool contains both GPTs, all 4 TMR modules and the TCK timer.
For a T4.0 and the settings from above the following code
```c++
PeriodicTimer t1,t2,t3,t4;
```
will generate timers using GPT1 (t1), GPT2 (t2), TMR1, ch1 (t3), TMR1, ch2 (t4).
You can change this settings to fit your needs.

**Pre-Scaling the TMR channels**   \
The TMR timers are clocked with 150MHz. Since these timers are only 16bit wide you'd get a maximum period of ```1/150MHz * 2^16 = 437µs```. For longer periods you can pre-scale the timer clock using the TMR_DEFAULT_PSC. Setting this to 0,1,2..7 leads to a pre-scale value of 1,2,4,...128 respectively. Using say 7 you'd get `128/150MHz = 0.853µs` per tick and a maximum period of `128/150MHz * 2^16 = 55.9ms`.

**Clock setting for the GPT and PIT timers**\
The USE_GPT_PID_150MHz setting determines if the GPT and PIT should use a 24MHz (default) or 150MHz clock. Please note that there is no possibility to change the clock for GPT or PIT individually. Changing this setting will also change the clock for the stock IntervalTimers.

**Callback Type**\
This setting allows for switching the standard std::function interface for callbacks to the traditional void (f*)() function pointer interface. In some cases the function pointer interfaces is more efficient than the std::function interface. But of course you will loose all the benefits described in the callback chapter above. 