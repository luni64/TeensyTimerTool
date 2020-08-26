#pragma once

#include "Arduino.h"
#include "TeensyTimerTool.h"

class PulsedDevice
{
 public:
    PulsedDevice() : pulseTimer(TeensyTimerTool::FTM0) {}  // use one of the 8 FTM0 channels
    void begin(unsigned pin, float delay, float duration);
    void triggerNow();                                     // switches pin on immediately, and switches it off after duration time, non blocking
    void triggerDelayed();                                 // switches pin on after delay time and leaves it on for the duration time, non blocking

 protected:
    uint32_t pin;
    uint32_t delayReload, durationReload;

    TeensyTimerTool::OneShotTimer pulseTimer;
    void callback();
};

void PulsedDevice::begin(unsigned pin, float delay, float duration)
{
    this->pin = pin;
    pinMode(pin, OUTPUT);

    pulseTimer.begin([this] { this->callback(); });
    pulseTimer.getTriggerReload(duration, &durationReload); // precalculate reload values to speed up trigger functions
    pulseTimer.getTriggerReload(delay, &delayReload);
}

void PulsedDevice::triggerDelayed()
{
    digitalWriteFast(pin, LOW);
    pulseTimer.triggerDirect(delayReload);
}

void PulsedDevice::triggerNow()
{
    digitalWriteFast(pin, HIGH);
    pulseTimer.triggerDirect(durationReload);
}

void PulsedDevice::callback()
{
    if (digitalReadFast(pin) == LOW)
    {
        digitalWriteFast(pin, HIGH);
        pulseTimer.triggerDirect(durationReload);
    } else
    {
        digitalWriteFast(pin, LOW);
    }
}

