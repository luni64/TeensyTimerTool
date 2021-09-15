#pragma once


namespace TeensyTimerTool
{
    class ITimerChannelEx;
    enum TimerModule : unsigned;
    enum class TimerType : unsigned;

    namespace TimerFactory
    {
        extern ITimerChannelEx *makeTimer(TimerModule, TimerType);
    }
}