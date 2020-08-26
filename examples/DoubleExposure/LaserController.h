#pragma once
#include "PulsedDevice.h"
#include "TeensyTimerTool.h"

class LaserController
{
 public:
    //LaserController();
    void begin(unsigned preTriggerPin, unsigned triggerPin, unsigned camPin);
    void shoot();

 protected:
    unsigned preTriggerPin, triggerPin, camPin;

    PulsedDevice preTrigger, trigger, camera;
};

// LaserController::LaserController(unsigned preTriggerPin, unsigned triggerPin, unsigned camPin)
// {
//     this->lamp = lamp;
//     this->qSwitch = qSwitch;
//     this->camera = camera;
// }

void LaserController::begin(unsigned preTriggerPin, unsigned triggerPin, unsigned camPin)
{
    constexpr unsigned warmUpTime = 139-4;
    constexpr unsigned switchPulseWidth = 10-2;
    constexpr unsigned camDelay = 109-5;
    constexpr unsigned camIntegrationTime = 30-2;

    preTrigger.begin(preTriggerPin, 0, switchPulseWidth);
    trigger.begin(triggerPin, warmUpTime, switchPulseWidth);
    camera.begin(camPin, camDelay, camIntegrationTime);
}

void LaserController::shoot()
{
    preTrigger.triggerNow();  // immediately generate a pretrigger pulse
    trigger.triggerDelayed(); // generate the trigger pulse after set delay time
    camera.triggerDelayed();  // generate the cam pulse after the set delay time
}
