#pragma once

#include "config.h"
#include <type_traits>

#if defined(USE_TIME_LITERALS)
#include "frequency.h"
#include <chrono>
#endif

namespace TeensyTimerTool
{
    //--------------------------------------------------------------
    // Transform arithmetic, period and frequency values to float
    // For constant inputs the transformations are calculated at compile time

    // Arithmetic types
    template <class period_t, std::enable_if_t<std::is_arithmetic<period_t>::value, int> * = nullptr>
    float constexpr period2us(period_t v)
    {
        return (float)v;
    }

#if defined(USE_TIME_LITERALS)

    // Duration types (s, ms, ns...)
    template <class period_t, std::enable_if_t<std::chrono::__is_duration<period_t>::value, int> * = nullptr>
    float constexpr period2us(period_t v)
    {
        return (duration_cast<duration<float, std::micro>>(v).count());
    }

    // Frequency types (Hz, MHz...)
    template <class period_t, std::enable_if_t<TeensyTimerTool::__is_frequency<period_t>::value, int> * = nullptr>
    float constexpr period2us(period_t v)
    {
        return 1'000'000 / duration_cast<hertz>(v).count();
    }
#endif

} // namespace TeensyTimerTool