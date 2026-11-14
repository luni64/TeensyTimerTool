#pragma once

namespace TeensyTimerTool
{
    enum class errorCode {
        OK = 0,

        // Warnings (negative): the function did its job with a restriction
        periodOverflow   = -100, // period / delay too long, the maximum is used
        wrongType        = -101, // not used
        triggeredLate    = -102, // not used
        prescalerIgnored = -103, // setPrescaler() before begin() on a timer without prescaler; the timer runs without

        // Errors (positive): the function could not do its job
        invalidArgument = 100, // negative period or delay, invalid prescaler
        noCallback      = 101, // empty callback (nullptr)
        zeroPeriod      = 102, // period 0
        poolExhausted   = 103, // the timer pool has no free timer
        noFreeChannel   = 104, // the requested timer type has no free channel
        notSupported    = 105, // the timer doesn't support this function
        notInitialized  = 106, // called before begin()

        // Names up to v1.4, kept so that existing sketches compile
        argument [[deprecated("use errorCode::invalidArgument")]]     = invalidArgument,
        callback [[deprecated("use errorCode::noCallback")]]          = noCallback,
        reload [[deprecated("use errorCode::zeroPeriod")]]            = zeroPeriod,
        noFreeModule [[deprecated("use errorCode::poolExhausted")]]   = poolExhausted,
        notImplemented [[deprecated("use errorCode::notSupported")]]  = notSupported,

        // GTP Errors
        GTP_err  = 200,
        GTP_err2 = 201,

        //TMR Errors
        TMR_err  = 300,
        TMR_err2 = 301,

        //FTM Errors
        FTM_err  = 400,
        FTM_err2 = 401,

        //TCK Errors
        TCK_err  = 900,
        TCK_err2 = 901,
    };
}