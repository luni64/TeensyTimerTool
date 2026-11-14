#pragma once

namespace TeensyTimerTool
{
    /// @brief Result of the timer functions: OK (0), warnings (negative: done with a restriction),
    /// errors (positive: not done, the timer is unchanged).
    /// @details Manual: https://luni64.github.io/TeensyTimerTool/error-handling/
    enum class errorCode {
        OK = 0, ///< success

        // Warnings (negative): the function did its job with a restriction
        periodOverflow   = -100, ///< warning: period / delay too long, the timer uses its maximum
        wrongType        = -101, ///< not used
        triggeredLate    = -102, ///< not used
        prescalerIgnored = -103, ///< warning: setPrescaler() before begin() on a timer without prescaler; the timer runs without

        // Errors (positive): the function could not do its job
        invalidArgument = 100, ///< error: negative period or delay, invalid prescaler value
        noCallback      = 101, ///< error: empty callback (nullptr)
        zeroPeriod      = 102, ///< error: period 0
        poolExhausted   = 103, ///< error: the timer pool has no free timer left
        noFreeChannel   = 104, ///< error: all channels of the requested timer type are in use
        notSupported    = 105, ///< error: the timer doesn't support this function
        notInitialized  = 106, ///< error: called before begin()

        // Names up to v1.4, kept so that existing sketches compile
        argument [[deprecated("use errorCode::invalidArgument")]]     = invalidArgument, ///< deprecated, use invalidArgument
        callback [[deprecated("use errorCode::noCallback")]]          = noCallback,      ///< deprecated, use noCallback
        reload [[deprecated("use errorCode::zeroPeriod")]]            = zeroPeriod,      ///< deprecated, use zeroPeriod
        noFreeModule [[deprecated("use errorCode::poolExhausted")]]   = poolExhausted,   ///< deprecated, use poolExhausted
        notImplemented [[deprecated("use errorCode::notSupported")]]  = notSupported,    ///< deprecated, use notSupported

        // reserved for internal errors of the timer modules
        GTP_err  = 200,
        GTP_err2 = 201,
        TMR_err  = 300,
        TMR_err2 = 301,
        FTM_err  = 400,
        FTM_err2 = 401,
        TCK_err  = 900,
        TCK_err2 = 901,
    };
}
