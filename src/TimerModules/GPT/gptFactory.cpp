#include "gptFactory.h"
#include "../boardDef.h"
#include "gptChannel.h"

namespace TeensyTimerTool
{
    namespace GPTFactory
    {
        namespace // private
        {
            void *gpt1 = nullptr;
            void *gpt2 = nullptr;
        } // namespace

        ITimerChannelEx *makeTimer(TimerModule module, TimerType type)
        {
            switch (module)
            {
                case TimerModule::GPT1:
                    if (gpt1) return nullptr;
                    gpt1 = new (std::nothrow) GptChannel<0>();
                    return (ITimerChannelEx *)gpt1;

                case TimerModule::GPT2:
                    if (gpt2) return nullptr;

                    gpt2 = new (std::nothrow) GptChannel<1>();
                    return (ITimerChannelEx *)gpt2;

                case TimerModule::GPT:
                    if (!gpt1)
                    {
                        gpt1 = new (std::nothrow) GptChannel<0>();
                        return (ITimerChannelEx *)gpt1;
                    }
                    if (!gpt2)
                    {
                        gpt2 = new (std::nothrow) GptChannel<1>();
                        return (ITimerChannelEx *)gpt2;
                    }
                    return nullptr;

                default:
                    return nullptr;
            }
        }

        void destroyTimer(void *t)
        {
            if (gpt1 == t)
                gpt1 = nullptr;
            else if (gpt2 == t)
                gpt2 = nullptr;
        }

    } // namespace GPTFactory
} // namespace TeensyTimerTool