#pragma once

namespace TeensyTimerTool
{
    enum class errorCode
    {
        OK =                 0,

        // Warnings
        periodOverflow=   -100,
        wrongType=        -101,

        //General errors
        argument =         100,
        callback=          101,
        reload=            102,
        noFreeModule =     103,
        noFreeChannel =    104,
        notImplemented=    105,
        notInitialized=    106,

        // GTP Errors
        GTP_err =          200,
        GTP_err2 =         201,

        //TMR Errors
        TMR_err =          300,
        TMR_err2 =         301,

        //FTM Errors
        FTM_err =          400,
        FTM_err2 =         401,

        //TCK Errors
        TCK_err =          900,
        TCK_err2 =         901,
    };
}