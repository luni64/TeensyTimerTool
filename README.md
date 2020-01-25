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
	* [How to Embed a Timer and its Callback in a Class](#HowtoEmbedaTimeranditsCallbackinaClass)
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
The following sketch shows the basic use of the TeensyTimerTool. It picks the next free timer from a pool of available timers and sets it so that the callback function is called periodically every 250ms.

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

### <a name='One-ShotTimer'></a>One-Shot Timer
To use the timer in one-shot mode, simply replace ``beginPeriodic`` with ``beginOneShot``. After starting a one-shot-timer with its ``trigger`` function the callback is called once after the delay time has expired.

The example below defines a one-shot timer whose callback simply switches off the board LED. In ``loop()``  the LED is switched on every 500ms. Immediately afterwards the timer is triggered with a delay time of 10ms so that the LED flashes briefly every 500ms.


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
Timer t1(GPT1);
Timer t2(GPT2);
```

### <a name='TMRakaQUADTimer'></a>TMR aka QUAD Timer
The QUAD modules (TMR) are 16 bit timer modules with 4 timer channels per module.
Teensy 4.0 controller has four TMR modules (TMR1 ... TMR4). The Teensyduino core uses TMR1-TMR3 for generating PWM signals. Using one of those will disable the PMW capability of the corresponding pins.

```c++
Timer t1(TMR1);  // first free channel of TMR1
Timer t2(TMR1);  // next free channel of TMR1 (up to 4)
Timer t3(TMR3);  // first free channel of TMR3
...
```

### <a name='PIT-PeriodicTimer'></a>PIT - Periodic Timer
Not yet implemented

### <a name='TCK-TickTimer'></a>TCK - Tick Timer
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

### <a name='LambdaExpressionsandCallbackswithContext'></a>Lambda Expressions and Callbacks with Context

Using lambda expressions as callbacks allows for some interesting use cases. If you are not familiar with lambdas, here a nice write up from Sandor Dago http://sandordargo.com/blog/2018/12/19/c++-lambda-expressions.

Let's start with a simple example:

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

Timer t1;

void setup()
{
    while(!Serial);

    t1.beginOneShot([] { Serial.printf("I'm called at %d ms\n", millis()); });

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

As you see, there is no need to create a dedicated callback function. You can directly define it as a lambda expression in the call to beginOneShot. That's nice, but not very exciting.

It gets more interesting when you need to pass context to a callback function:

```c++
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

elapsedMillis stopwatch;
Timer t1, t2, t3;

void callbackWithContext(unsigned pin)
{
    int callTime = stopwatch;
    Serial.printf("Do something with pin %u (%u ms)\n", pin, callTime);
}

void setup()
{
    while(!Serial);

     t1.beginOneShot([] { callbackWithContext(1); });
     t2.beginOneShot([] { callbackWithContext(2); });
     t3.beginOneShot([] { callbackWithContext(3); });

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
It is quite tedious to achieve the same with traditional function pointer callbacks.

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

// copy Bliker class definition here or use separate .h file

Timer t1, t2, t3;

Blinker b1(1);           // blinks on pin 1
Blinker b2(7);           // blinks on pin 7
Blinker b3(LED_BUILTIN); // blinks the built in LED

void setup()
{
    t1.beginPeriodic([] { b1.blink(); }, 1'000);
    t2.beginPeriodic([] { b2.blink(); }, 2'000);
    t3.beginPeriodic([] { b3.blink(); }, 50'000);
}

void loop() {}
```



### <a name='HowtoEmbedaTimeranditsCallbackinaClass'></a>How to Embed a Timer and its Callback in a Class
Embedding a timer and its callback function in a class can attractive if you want to hide implementation details from the users of the class. However, setting this up can be quite tedious if the timer expects the usual pointer to a void function as callback. The reason for the complicaton is, that every member function carries a pointer to the object as a hidden and auto generated first parameter. Thus, the signature of a (not static) member function can never match the required void(*f)() and you'll get errors if you try to attach it as callback.

Since the TeensyTimerTool timers accept a ```std::function<void>()``` argument as callback, attaching member functions is straight forward. Basically you have the following two options:
- Simply attach a lambda which captures the 'this' pointer and uses it to call the member. Here an example for this method:
```c++
    timer.beginOneShot([this] {this->myMemberFunction();});
```
- Use std::bind to bind the hidden first parameter of the member to 'this' as shown below. The somehow ugly syntax is explained [here](https://stackoverflow.com/a/7582576/1842762)
```c++
    timer.beginOneShot(std::bind(&MyClass::myMemberFunction, this));
```

The following working example shows a *Blinker* class which uses an embedded timer to periodically toggle a pin in the background.
You can generate as many Blinker objects as you like (as long as you don't run out of timers). They will do their work autonomously, the user does not need to know anything about the details.

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
        pinMode(pin, OUTPUT);
        timer.beginPeriodic([this] {this->blink();}, period );  //  using a lambda
       // timer.beginPeriodic(std::bind(&Blinker::blink, this), period); // using std::bind
    }

    void blink() // callback function
    {
        digitalWriteFast(pin, !digitalReadFast(pin));
    }

protected:
    Timer timer;
    unsigned pin, period;
};

//==============================================================

Blinker b1(1, 100'000); // blinks on pin 1 with period 100ms
Blinker b2(2, 50'000);  // blinks on pin 2 with period  50ms

void setup()
{
    b1.begin();
    b2.begin();
}

void loop()
{
}
```

## <a name='ErrorHandling'></a>Error Handling

Things can go wrong in programming. E.g. assume that you try to acquire three channels of  an FTM1 module which only provides two channels
```c++
Timer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    t1.beginOneShot(callback1);
    t2.beginOneShot(callback2);
    t3.beginOneShot(callback3);
}
...
```
This will lead to a runtime error when you try to initialize t3 in the last line. TeensyTimerTool won't crash on it but t3 will never work of course.

### <a name='ManuallyCheckforErrors'></a>Manually Check for Errors

So, it is a good idea to check if the acquisition of a timer didn't produce an error. Here an example showing how this can be done. In case of an error a ```panic``` function will be called which simply prints out the error and enters an endless, fast blink loop to signal a problem.

```c++
Timer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    ...
    errorCode err;
    err = t1.beginOneShot(callback1);
    if (err != errorCode::OK)
    {
        panic(err);
    }

    err = t2.beginOneShot(callback2);
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
Timer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    Timer::attachErrFunc(panic);

    while(!Serial);
    pinMode(LED_BUILTIN, OUTPUT);

    t1.beginOneShot(callback1);
    t2.beginOneShot(callback2);
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
Timer t1(FTM1), t2(FTM1), t3(FTM1)

void setup()
{
    Timer::attachErrFunc(ErrorHandler(Serial));

    while(!Serial);
    pinMode(LED_BUILTIN, OUTPUT);

    t1.beginOneShot(callback1);
    t2.beginOneShot(callback2);
}
...
```

Output:
```
Error: 104: Timer module has no free channel
```


## <a name='Configuration'></a>Configuration
tbd

