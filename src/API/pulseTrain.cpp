//#include "Arduino.h"
#include "pulseTrain.h"
#include "core_pins.h"
#include "types.h"
#include <algorithm>
#include <cstdlib>

namespace TeensyTimerTool
{
    PulseTrain::PulseTrain(Pulse *pulses, size_t nrOfPulses)
    {
        SetPulses(pulses, nrOfPulses);
    }

    PulseTrain::PulseTrain(std::initializer_list<Pulse> pulses)
    {
        // calculate smallest possible prescaler for the pulse train
        psc         = 0;
        float max   = 0;
        float limit = UINT16_MAX * 1E6f / F_BUS_ACTUAL;
        float calib = 0;

        for (const Pulse &pulse : pulses)
        {
            auto current = std::max(pulse.hi.us, pulse.lo.us);
            if (current > max)
            {
                while (current > limit)
                {
                    psc++;
                    limit *= 2;
                    if (psc == 8)
                    {
                        errState = errorCode::pulseLengthOverflow;
                        return;
                    }
                }
            }
            max = current;
        }
        calib = UINT16_MAX / limit;

        // calculate counter ticks from state durations, store in the DMA buffers and empty the corresponding cache lines
        bufSize = pulses.size();
        allocDMABuffers(bufSize - 2);
        //Serial.printf("alloc size: %d\n", bufSize - 2);

        const Pulse *pulse = pulses.begin();

        h0 = pulse->hi.us * calib;
        l0 = pulse->lo.us * calib;
        pulse++;
        h1 = pulse->hi.us * calib;
        l1 = pulse->lo.us * calib;
        pulse++;

        //Serial.printf("0, hi: %d lo: %d\n", h0, l0);
        //Serial.printf("1, hi: %d lo, %d\n", h1, l1);

        int i = 0;
        while (pulse != pulses.end())
        {
            hiBuffer->data[i] = pulse->hi.us * calib;
            loBuffer->data[i] = pulse->lo.us * calib;
            //Serial.printf("%d, hi: %d lo: %d psc: %d\n", i + 2, hiBuffer->data[i], loBuffer->data[i], psc);
            i++;
            pulse++;
        }
        loBuffer->flushCache();
        hiBuffer->flushCache();
    }

    void PulseTrain::allocDMABuffers(unsigned size)
    {
        loBuffer.reset();
        hiBuffer.reset();
        loBuffer = std::make_shared<DMABuffer>(size);
        hiBuffer = std::make_shared<DMABuffer>(size);
    }

    errorCode PulseTrain::SetPulses(Pulse *pulses, size_t len)
    {
        bufSize = len;

        // calculate smallest possible prescaler for the pulse train
        psc         = 0;
        float max   = 0;
        float limit = UINT16_MAX * 1E6f / F_BUS_ACTUAL;
        float calib = 0;

        for (size_t i = 0; i < len; i++)
        {
            float current = std::max(pulses[i].hi.us, pulses[i].lo.us);
            //Serial.printf("c: %f, m: %f\n", current, max);
            if (current > max)
            {
                while (current > limit)
                {
                    psc++;
                    limit *= 2;
                    if (psc == 8) return errorCode::pulseLengthOverflow;
                }
            }
            max = current;
        }
        calib = UINT16_MAX / limit;

        //Serial.printf("psc: %d, calib: %f, limit:%f", psc, calib, limit);

        // calculate timer ticks from state durations, store in the DMA buffers and empty the corresponding cache lines
        allocDMABuffers(bufSize - 2);

        h0 = pulses[0].hi.us * calib;
        l0 = pulses[0].lo.us * calib;
        h1 = pulses[1].hi.us * calib;
        l1 = pulses[1].lo.us * calib;

        // Serial.printf("0, hi: %d lo: %d\n", h0, l0);
        // Serial.printf("1, hi: %d lo: %d\n", h1, l1);

        for (size_t i = 0; i < bufSize - 2; i++)
        {
            Pulse &pulse      = pulses[2 + i];
            hiBuffer->data[i] = pulse.hi.us * calib;
            loBuffer->data[i] = pulse.lo.us * calib;
            //Serial.printf("%d, hi: %d lo: %d\n", i + 2, hiBuffer->data[i], loBuffer->data[i]);
        }
        loBuffer->flushCache();
        hiBuffer->flushCache();

        return errorCode::OK;
    }

} // namespace TeensyTimerTool
