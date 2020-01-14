#pragma once

namespace TeensyTimerTool
{
    enum class error {
        OK,                  // 0
        argument,            // 1
        noFreeModule,        // 2
        noFreeChannel,       // 3

        // GTP Errors
        GTP_err = 100,       // 100
        GTP_err2,            // 101
    };
}