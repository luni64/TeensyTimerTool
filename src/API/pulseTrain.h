#pragma once

#include "ErrorHandling/error_codes.h"
#include "Utility/DMABuffer.h"
#include <chrono>
#include <initializer_list>
#include <memory>

namespace TeensyTimerTool
{
    //==============================================================================================================
    // Helper class. Converts arithmetic types and chrono::durations to microseconds (float).
    // All values known at compiletime are converted at compiletime

    struct PulseTiming
    {
        constexpr PulseTiming() : us(0.0f) {}

        template <typename period_t, std::enable_if_t<std::is_arithmetic<period_t>::value, int> * = nullptr>
        constexpr PulseTiming(period_t p) : us((float)(p)) {}

        template <typename period_t, std::enable_if_t<std::chrono::__is_duration<period_t>::value, int> * = nullptr>
        constexpr PulseTiming(period_t p) : us(std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(p).count()) {}

        float us;
    };

    //==============================================================================================================
    // complete pulse, always starts with high level
    struct Pulse
    {
        PulseTiming hi;
        PulseTiming lo;
    };

    //==============================================================================================================
    // Pulsetrain objects can be passed to the trigger function of the pulse generator classes. The class transforms
    // the various pulse inputs (initializer_list of Pulse objects, array of Pulse objects) to a list of uint16_t compare values
    // needed by the hardware counters. Those values are stored in a 32bit aligned 'DMA Buffer'.
    //
    // PulseGenerator objects maintains a shared_ptr to the DMA Buffer. Thus,
    // - the same pulse train can be used for more than one generator
    // - the lifetime of the PulseTrain object may be shorter than the lifetime
    //   of the pulse generator objecs using the train without the need for costly
    //   copy operations.

    using DMABuffer       = dmaBuffer_t<uint16_t>;
    using sharedDMABuffer = std::shared_ptr<DMABuffer>;

    class PulseTrain
    {
     public:
        PulseTrain(std::initializer_list<Pulse> pulses);
        PulseTrain(Pulse *pulses, size_t nrOfPulses);
        errorCode SetPulses(Pulse *pulses, size_t nrOfPulses);  

        uint8_t psc; // todo: define getters for the fields
        float calib;
        size_t bufSize     = 0;
        errorCode errState = errorCode::OK;

        uint16_t h0, l0, h1, l1;
        sharedDMABuffer getLoBuffer() const { return loBuffer; }
        sharedDMABuffer getHiBuffer() const { return hiBuffer; }

     protected:
        sharedDMABuffer loBuffer; // consumer of the buffers might exist longer
        sharedDMABuffer hiBuffer; // than this pulse train -> share ownership
        void allocDMABuffers(unsigned size);
    };

} // namespace TeensyTimerTool