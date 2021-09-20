#pragma once
namespace TeensyTimerTool
{
    namespace GptHelpers
    {
        extern float getMaxMicros();
        extern float getMaxPeriod();
        extern uint32_t microsecondToCycles(float micros);

    }; // namespace GptHelpers

    struct IMXRT_GPT_t
    {
        volatile uint32_t CR;   //51.7.1  GPT Control Register
        volatile uint32_t PR;   //51.7.2  GPT Prescaler Register(GPTx_PR)
        volatile uint32_t SR;   //51.7.3  GPT Status Register(GPTx_SR)
        volatile uint32_t IR;   //51.7.4  GPT Interrupt Register(GPTx_IR)
        volatile uint32_t OCR1; //51.7.5  GPT Output Compare Register  (GPTx_OCR1)
        volatile uint32_t OCR2; //51.7.6  GPT Output Compare Register  (GPTx_OCR2)
        volatile uint32_t OCR3; //51.7.7  GPT Output Compare Register  (GPTx_OCR3)
        volatile uint32_t ICR1; //51.7.8  GPT Input Capture Register 1 (GPTx_ICR1)
        volatile uint32_t ICR2; //51.7.9  GPT Input Capture Register 1 (GPTx_ICR2)
        volatile uint32_t CNT;  //51.7.10 GPT Counter Register (GPTx_CNT)
    };

} // namespace TeensyTimerTool