#pragma once

#include <stdint.h>
#include <core_pins.h>
#include <type_traits>

namespace pins
{
    namespace // private
    {
        enum boards {
            notDefined = -1,
            T_LC,
            T3_0_1_2,
            T3_5_6,
        };

#if defined(__MKL26Z64__)
        constexpr boards board = boards::T_LC;
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
        constexpr boards board = boards::T3_0_1_2;
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
        constexpr boards  board = boards::T3_5_6;
#else
        constexpr boards  board = boards::notDefined;
#endif
        static_assert(board != boards::notDefined, "Error in Pin.h");

        // Indices into GPIOx and PORT arrays below
        enum portList {
            na = -1, Port_A, Port_B, Port_C, Port_D, Port_E
        };

        //Base adresses of the GPIOx register blocks
        constexpr uintptr_t GPIOx_PDOR[] =
        {
            (uintptr_t)&(GPIOA_PDOR),
            (uintptr_t)&(GPIOB_PDOR),
            (uintptr_t)&(GPIOC_PDOR),
            (uintptr_t)&(GPIOD_PDOR),
            (uintptr_t)&(GPIOE_PDOR),
        };

        // Base adresses of the Pin Control Registers
        constexpr uintptr_t PORTx_PCR0[] =
        {
            (uintptr_t)&(PORTA_PCR0),
            (uintptr_t)&(PORTB_PCR0),
            (uintptr_t)&(PORTC_PCR0),
            (uintptr_t)&(PORTD_PCR0),
            (uintptr_t)&(PORTE_PCR0),
        };

        //----------------------------------------------------------------
        // Translate Teensy pin number to port and bitnr
        // The map will be used by the complier only, no code will be generated

        struct pinInfo {
            const portList port;
            const int pin;
        };

        constexpr pinInfo pinMap[][3] =
        {  //                 T-LC            T3.0/1/2        T3.5/6
            /* Pin 0 */{ { Port_B, 16 },{ Port_B, 16 },{ Port_B, 16 } },
            /* Pin 1 */{ { Port_B, 17 },{ Port_B, 17 },{ Port_B, 17 } },
            /* Pin 2 */{ { Port_D,  0 },{ Port_D,  0 },{ Port_D,  0 } },
            /* Pin 3 */{ { Port_A,  1 },{ Port_A, 12 },{ Port_A, 12 } },
            /* Pin 4 */{ { Port_A,  2 },{ Port_A, 13 },{ Port_A, 13 } },
            /* Pin 5 */{ { Port_D,  7 },{ Port_D,  7 },{ Port_D,  7 } },
            /* Pin 6 */{ { Port_D,  4 },{ Port_D,  4 },{ Port_D,  4 } },
            /* Pin 7 */{ { Port_D,  2 },{ Port_D,  2 },{ Port_D,  2 } },
            /* Pin 8 */{ { Port_D,  3 },{ Port_D,  3 },{ Port_D,  3 } },
            /* Pin 9 */{ { Port_C,  3 },{ Port_C,  3 },{ Port_C,  3 } },
            /* Pin 10*/{ { Port_C,  4 },{ Port_C,  4 },{ Port_C,  4 } },
            /* Pin 11*/{ { Port_C,  6 },{ Port_C,  6 },{ Port_C,  6 } },
            /* Pin 12*/{ { Port_C,  7 },{ Port_C,  7 },{ Port_C,  7 } },
            /* Pin 13*/{ { Port_C,  5 },{ Port_C,  5 },{ Port_C,  5 } },
            /* Pin 14*/{ { Port_D,  1 },{ Port_D,  1 },{ Port_D,  1 } },
            /* Pin 15*/{ { Port_C,  0 },{ Port_C,  0 },{ Port_C,  0 } },
            /* Pin 16*/{ { Port_B,  0 },{ Port_B,  0 },{ Port_B,  0 } },
            /* Pin 17*/{ { Port_B,  1 },{ Port_B,  1 },{ Port_B,  1 } },
            /* Pin 18*/{ { Port_B,  3 },{ Port_B,  3 },{ Port_B,  3 } },
            /* Pin 19*/{ { Port_B,  2 },{ Port_B,  2 },{ Port_B,  2 } },
            /* Pin 20*/{ { Port_D,  5 },{ Port_D,  5 },{ Port_D,  5 } },
            /* Pin 21*/{ { Port_D,  6 },{ Port_D,  6 },{ Port_D,  6 } },
            /* Pin 22*/{ { Port_C,  1 },{ Port_C,  1 },{ Port_C,  1 } },
            /* Pin 23*/{ { Port_C,  2 },{ Port_C,  2 },{ Port_C,  2 } },
            /* Pin 24*/{ { Port_E, 20 },{ Port_A,  5 },{ Port_E, 26 } },
            /* Pin 25*/{ { Port_E, 21 },{ Port_B, 19 },{ Port_A,  5 } },
            /* Pin 26*/{ { Port_E, 30 },{ Port_E,  1 },{ Port_A, 14 } },
            /* Pin 27*/{ { na,     -1 },{ Port_C,  9 },{ Port_A, 15 } },
            /* Pin 28*/{ { na,     -1 },{ Port_C,  8 },{ Port_A, 16 } },
            /* Pin 29*/{ { na,     -1 },{ Port_C, 10 },{ Port_B, 18 } },
            /* Pin 30*/{ { na,     -1 },{ Port_C,  5 },{ Port_B, 19 } },
            /* Pin 31*/{ { na,     -1 },{ Port_E,  6 },{ Port_B, 10 } },
            /* Pin 32*/{ { na,     -1 },{ Port_B,  1 },{ Port_B, 11 } },
            /* Pin 33*/{ { na,     -1 },{ Port_A,  2 },{ Port_E, 24 } },
            /* Pin 34*/{ { na,     -1 },{ na,     -1 },{ Port_E, 25 } },
            /* Pin 35*/{ { na,     -1 },{ na,     -1 },{ Port_C,  8 } },
            /* Pin 36*/{ { na,     -1 },{ na,     -1 },{ Port_C,  9 } },
            /* Pin 37*/{ { na,     -1 },{ na,     -1 },{ Port_C, 10 } },
            /* Pin 38*/{ { na,     -1 },{ na,     -1 },{ Port_C, 11 } },
            /* Pin 39*/{ { na,     -1 },{ na,     -1 },{ Port_A, 17 } },
            /* Pin 40*/{ { na,     -1 },{ na,     -1 },{ Port_A, 28 } },
            /* Pin 41*/{ { na,     -1 },{ na,     -1 },{ Port_A, 29 } },
            /* Pin 42*/{ { na,     -1 },{ na,     -1 },{ Port_A, 26 } },
            /* Pin 43*/{ { na,     -1 },{ na,     -1 },{ Port_B, 20 } },
            /* Pin 44*/{ { na,     -1 },{ na,     -1 },{ Port_B, 22 } },
            /* Pin 45*/{ { na,     -1 },{ na,     -1 },{ Port_B, 23 } },
            /* Pin 46*/{ { na,     -1 },{ na,     -1 },{ Port_B, 21 } },
            /* Pin 47*/{ { na,     -1 },{ na,     -1 },{ Port_D,  8 } },
            /* Pin 48*/{ { na,     -1 },{ na,     -1 },{ Port_D,  9 } },
            /* Pin 49*/{ { na,     -1 },{ na,     -1 },{ Port_B,  4 } },
            /* Pin 50*/{ { na,     -1 },{ na,     -1 },{ Port_B,  5 } },
            /* Pin 51*/{ { na,     -1 },{ na,     -1 },{ Port_D, 14 } },
            /* Pin 52*/{ { na,     -1 },{ na,     -1 },{ Port_D, 13 } },
            /* Pin 53*/{ { na,     -1 },{ na,     -1 },{ Port_D, 12 } },
            /* Pin 54*/{ { na,     -1 },{ na,     -1 },{ Port_D, 15 } },
            /* Pin 55*/{ { na,     -1 },{ na,     -1 },{ Port_D, 11 } },
            /* Pin 56*/{ { na,     -1 },{ na,     -1 },{ Port_E, 10 } },
            /* Pin 57*/{ { na,     -1 },{ na,     -1 },{ Port_E, 11 } },
            /* Pin 58*/{ { na,     -1 },{ na,     -1 },{ Port_E,  0 } },
            /* Pin 59*/{ { na,     -1 },{ na,     -1 },{ Port_E,  1 } },
            /* Pin 60*/{ { na,     -1 },{ na,     -1 },{ Port_E,  2 } },
            /* Pin 61*/{ { na,     -1 },{ na,     -1 },{ Port_E,  3 } },
            /* Pin 62*/{ { na,     -1 },{ na,     -1 },{ Port_E,  4 } },
            /* Pin 63*/{ { na,     -1 },{ na,     -1 },{ Port_E,  5 } },
        };
    }

    // Pin Mux constants
    enum pinMux
    {
        ALT0 = 0,
        ALT1, ALT2, ALT3, ALT4, ALT5, ALT6, ALT7
    };

    //-----------------------------------------------------------------------------------------------------
    // Pin class
    // all methods static, no instances will be generated in code

    template <unsigned pinNr>
    class pin
    {
    private:
        // Calculate the adress of the pin control register
        static constexpr uintptr_t get_PORTx_PCRn_addr()
        {
            static_assert(pinMap[pinNr][board].port != na && pinMap[pinNr][board].pin != -1, "Pin not supported by board");
            return PORTx_PCR0[(pinMap[pinNr][board]).port] + 4 * pinMap[pinNr][board].pin;
        }

        // Calculate the adress of the bit banded GPIO registers of the pin
        static constexpr uintptr_t getPDORAdr()
        {
            return 0x42000000 + 32 * (GPIOx_PDOR[pinMap[pinNr][board].port] - 0x40000000) + 4 * pinMap[pinNr][board].pin;
        }

        static constexpr uintptr_t pcr = get_PORTx_PCRn_addr(); // Pin Control Register
        static constexpr uintptr_t pdorBB = getPDORAdr();       // Bitband adress of GPIOx_PDOR
        static constexpr uintptr_t psorBB = pdorBB + 4 * 32;    //     GPIOx_PSOR = GPIOx_PDOR + 4
        static constexpr uintptr_t pcorBB = psorBB + 4 * 32;    //     GPIOx_PCOR = GPIOx_PDOR + 8
        static constexpr uintptr_t ptorBB = pcorBB + 4 * 32;    //     GPIOx_PTOR = GPIOx_PDOR + 12
        static constexpr uintptr_t pdirBB = ptorBB + 4 * 32;    //     GPIOx_PDIR = GPIOx_PDOR + 16
        static constexpr uintptr_t pddrBB = pdirBB + 4 * 32;    //     GPIOx_PDDR = GPIOx_PDOR + 20

    public:
        // Construction (class contains only static members)
        pin() {}
        pin(int i) { pinMode(i); }

        pin(const pin&) = delete;  // disable copy constructor

        // Setting and getting the pin value
        template<class T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
        operator T() const { return *reinterpret_cast<volatile uint32_t*>(pdirBB); }
        operator bool() const { return *reinterpret_cast<volatile uint32_t*>(pdirBB) & 1; }
        inline void operator = (const bool v) const { *reinterpret_cast<volatile uint32_t*>(pdorBB) = v; } // assignment  --> somePin = HIGH
        inline void operator = (const pin& v) const = delete;                                             // disable copying

        // Pin operations
        static inline void toggle() { *reinterpret_cast<volatile uint32_t*>(ptorBB) = 1; }                // toggles pin
        //static inline int  getValue() { return *reinterpret_cast<volatile uint32_t*>(pdirBB); }           // returns pin value (usefull for static calls)
        //static inline void setValue(const int v) { *reinterpret_cast<volatile uint32_t*>(pdorBB) = v; }   // sets value (usefull for static calls)
        //static inline void setHIGH() { *reinterpret_cast<volatile uint32_t*>(psorBB) = 1; }               // sets pin to HIGH (usefull for static calls)
        //static inline void setLOW() { *reinterpret_cast<volatile uint32_t*>(pcorBB) = 1; }                // sets pin to LOW (usefull for static calls)

        // Pin configuration
        static inline void pinMode(int mode)
        {
            // replace by a call to pinMode(pinNr,mode) ??? TBD
            switch (mode)
            {
            case OUTPUT:
                *reinterpret_cast<volatile uint32_t*>(pddrBB) = 1;                                          //Pin direction register
                *reinterpret_cast<volatile uint32_t*>(pcr) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1); //Pin control register
                break;
            case OUTPUT_OPENDRAIN:
                *reinterpret_cast<volatile uint32_t*>(pddrBB) = 1;
                *reinterpret_cast<volatile uint32_t*>(pcr) = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1) | PORT_PCR_ODE;
                break;
            case INPUT:
                *reinterpret_cast<volatile uint32_t*>(pddrBB) = 0;
                *reinterpret_cast<volatile uint32_t*>(pcr) = PORT_PCR_MUX(1);
                break;
            case INPUT_PULLUP:
                *reinterpret_cast<volatile uint32_t*>(pddrBB) = 0;
                *reinterpret_cast<volatile uint32_t*>(pcr) = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
                break;
            case INPUT_PULLDOWN:
                *reinterpret_cast<volatile uint32_t*>(pddrBB) = 0;
                *reinterpret_cast<volatile uint32_t*>(pcr) = PORT_PCR_MUX(1) | PORT_PCR_PE;
                break;
            case INPUT_DISABLE:
                *reinterpret_cast<volatile uint32_t*>(pcr) = 0;
                break;
            }
        }

        static inline void driveStrengthEnable(bool enable)
        {
            //TBD static_assert: pin has DSE capability
            if (enable) {
                *reinterpret_cast<uint32_t*>(pcr) |= PORT_PCR_DSE;
            }
            else {
                *reinterpret_cast<uint32_t*>(pcr) &= ~PORT_PCR_DSE;
            }
        }

        static inline void slowSlewRateEnable(bool enable)
        {
            //TBD static_assert: pin has SRE capability
            if (enable) {
                *reinterpret_cast<uint32_t*>(pcr) |= PORT_PCR_SRE;
            }
            else {
                *reinterpret_cast<uint32_t*>(pcr) &= ~PORT_PCR_SRE;
            }
        }

        static inline void setMUX(pinMux altFunc)
        {
            *reinterpret_cast<uint32_t*>(pcr) = (*reinterpret_cast<uint32_t*>(pcr) & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(altFunc);
        }

        static inline void attachInterrupt(void(*function)(void), int mode)
        {
            ::attachInterrupt(pinNr, function, mode);
        }
    };
}