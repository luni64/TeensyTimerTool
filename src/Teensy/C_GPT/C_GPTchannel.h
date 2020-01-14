#pragma once

#include "../ITimerChannel.h"
#include "../types.h"
#include "C_GPTmap.h"


namespace TeensyTimerTool
{
    class C_GptChannel : public ITimerChannel
    {
     public:
        inline C_GptChannel(volatile IMXRT_GPT_t*, uint32_t chNr, callback_t* cbStorage, uint32_t* rlStorage);
        inline virtual ~C_GptChannel();

        inline void begin(callback_t cb, unsigned tcnt, bool periodic);
        inline void trigger(uint32_t){}

     protected:
        volatile uint32_t *OCR, *CNT, *IR;

        uint32_t* pReload;
        //IMXRT_GPT_t* regs;
        uint32_t IRFlag;
        callback_t** pCallback = nullptr;
    };

    // IMPLEMENTATION ==============================================

    C_GptChannel::C_GptChannel(volatile IMXRT_GPT_t* regs, uint32_t chNr, callback_t* cbStorage, uint32_t* rlStorage)
        : ITimerChannel(cbStorage + chNr)
    {
        OCR = &regs->OCR1 + chNr;
        CNT = &regs->CNT;
        IR = &regs->IR;
        IRFlag = GPT_IR_OF1IE << chNr;
        pReload = rlStorage + chNr;
        Serial.printf("GptChannel() %p ch:%d\n", regs, IRFlag);
    }

    void C_GptChannel::begin(callback_t cb, unsigned reload, bool periodic)
    {
        setCallback(cb);
      //  *pReload = reload * 0.9726;
        *pReload = reload;
        *OCR = *CNT + reload;
        *IR |= IRFlag;
    }

    C_GptChannel::~C_GptChannel()
    {
        *IR &= ~IRFlag;
        *pCallback = (callback_t*)-1;
    }

} // namespace TeensyTimerTool