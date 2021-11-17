#include "PlsChannelTCK.h"
#include "API/PulseTrain.h"
#include "Arduino.h"

struct pwm_pin_info_struct
{
    uint8_t type;          // 0=no pwm, 1=flexpwm, 2=quad
    uint8_t module;        // 0-3, 0-3
    uint8_t channelNumber; // 0=X, 1=A, 2=B
    uint8_t muxval;        //
};
extern const struct pwm_pin_info_struct pwm_pin_info[];

namespace TeensyTimerTool
{
    PulseChannelTCK::PulseChannelTCK(uint8_t _pin)
        : pin(_pin), dmaChnLo(false), dmaChnHi(false)
    {
        auto pinInfo = pwm_pin_info[pin];
        uint8_t mNr  = 1 + (pinInfo.module >> 4);
        uint8_t chNr = pinInfo.module & 0b00001111;

        if (pinInfo.type == 2) // pin is managed by a TMR timer
        {
            moduleInfo_t moduleInfo = mNr == 1 ? getModuleInfo<1>()
                                    : mNr == 2 ? getModuleInfo<2>()
                                    : mNr == 3 ? getModuleInfo<3>()
                                               : getModuleInfo<4>();

            regs              = moduleInfo.channelInfo[chNr].registers;
            DMA_source_CMPLD1 = moduleInfo.channelInfo[chNr].DMA_source_cmpld1;
            DMA_source_CMPLD2 = moduleInfo.channelInfo[chNr].DMA_source_cmpld2;
        }
    }

    errorCode PulseChannelTCK::begin()
    {
        pinMode(pin, OUTPUT);
        digitalWriteFast(pin, LOW); // prevent unwanted peaks when switching between timer pin and GPIO

        // setup timer module
        regs->CTRL   = 0x0000;                                // disable counter
        regs->LOAD   = 0x0000;                                // reload counter with 0 after each compare event
        regs->CSCTRL = TMR_CSCTRL_CL1(1) | TMR_CSCTRL_CL2(2); // reload  COMP1/2 from CMPLD1/2 after corresponding compare events
        regs->DMA    = TMR_DMA_CMPLD1DE | TMR_DMA_CMPLD2DE;   // load new values for CMPLD1/2 using DMA

        // setup DMA channels
        dmaChnLo.begin();
        dmaChnLo.triggerAtHardwareEvent(DMA_source_CMPLD1); // trigger channel when a new CMPLD1 value is needed
        dmaChnLo.destination(regs->CMPLD1);                 // move this value to the CMPLD1 register
        dmaChnLo.interruptAtCompletion();                   // finishes the pulse train in this interrupt
        dmaChnLo.attachInterrupt(DMA_ISRL, this);           // we can pass state, since dmaChnLo is a DMAChannelEx object
        dmaChnLo.disableOnCompletion();

        dmaChnHi.begin();
        dmaChnHi.triggerAtHardwareEvent(DMA_source_CMPLD2); // trigger channel when a new CMPLD2 value is needed
        dmaChnHi.destination(regs->CMPLD2);                 // move this value to the CMPLD2 register
        dmaChnHi.interruptAtCompletion();                   // finishes the pulse train in this interrupt
        dmaChnHi.attachInterrupt(DMA_ISRH, this);           // we can pass state, since dmaChnLo is a DMAChannelEx object
        dmaChnHi.disableOnCompletion();

        return errorCode::OK;
    }

    errorCode PulseChannelTCK::trigger(const PulseTrain &pulseTrain)
    {
        if (pulseTrain.errState != errorCode::OK) return pulseTrain.errState;

        unsigned bufSize = pulseTrain.bufSize * sizeof(uint16_t);

        bufLo = pulseTrain.getLoBuffer();
        bufHi = pulseTrain.getHiBuffer();
        dmaChnLo.sourceBuffer(bufLo->data, bufSize);
        dmaChnHi.sourceBuffer(bufHi->data, bufSize);
        dmaChnHi.enable();
        dmaChnLo.enable();

        regs->CNTR   = 0;             // reset counter (just to be sure)
        regs->COMP1  = pulseTrain.l0; // preset the current comperators (starting values)
        regs->COMP2  = pulseTrain.h0;
        regs->CMPLD1 = pulseTrain.l1; // preset comperator load registers (second values), DMA will fetch the
        regs->CMPLD2 = pulseTrain.h1; // next relaod values (3..n) after tranfering these ones to the COMP1/2 registers

        regs->SCTRL = TMR_SCTRL_OEN; // directly output to pin
        delayMicroseconds(10);

        *(portConfigRegister(pin)) = 0x10 | 0x01; // route the pin to the timer output flag
        regs->CTRL                 = TMR_CTRL_CM(0b001) | TMR_CTRL_PCS(0b1000 | pulseTrain.psc) | TMR_CTRL_LENGTH | TMR_CTRL_OUTMODE(0b100);

        // digitalWriteFast(0, HIGH);
        // digitalWriteFast(1, HIGH);

        return errorCode::OK;
    }

    void PulseChannelTCK::DMA_ISRH(PulseChannelTCK *THIS)
    {
        //digitalWriteFast(1, LOW);
        THIS->dmaChnHi.clearInterrupt();
        *(portConfigRegister(THIS->pin)) = 0x10 | 0x05; // switch back to GPIO to prevent output of HIGH  after next compare event
        //Serial.println("doneH");
    }

    void PulseChannelTCK::DMA_ISRL(PulseChannelTCK *THIS)
    {
        //digitalWriteFast(0, LOW);
        THIS->regs->CTRL = 0; // last low period done -> disable timer
        THIS->dmaChnLo.clearInterrupt();

        THIS->bufLo.reset(); // release the buffers (shared_ptr)
        THIS->bufHi.reset();
        //Serial.println("doneL");
    }

} // namespace TeensyTimerTool