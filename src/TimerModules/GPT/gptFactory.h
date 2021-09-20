#pragma once

enum TimerModule : unsigned;
enum TimerType : unsigned;

namespace TeensyTimerTool
{
    class ITimerChannelEx;

    namespace GPTFactory
    {
        extern ITimerChannelEx *makeTimer(TimerModule, TimerType);
        void destroyTimer(void *);

    }; // namespace GPTFactory
} // namespace TeensyTimerTool
