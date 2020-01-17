#if defined(__MKL26Z64__) && defined(TEENSYDUINO)
#define TLC

#elif defined(__MK20DX128__) && defined(TEENSYDUINO)
#define T3_2

#elif defined(__MK20DX256__) && defined(TEENSYDUINO)
#define T3_2

#elif defined(__MK64FX512__) && defined(TEENSYDUINO)
#define T3_5

#elif defined(__MK66FX1M0__) && defined(TEENSYDUINO)
#define T3_6

#elif defined(__IMXRT1062__) && defined(TEENSYDUINO)
#define T4_0

#elif defined(ESP32)
//...

#else
#error "Board not supported"
#endif