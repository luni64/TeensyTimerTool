#if defined(__MKL26Z64__) && defined(TEENSYDUINO)
#define TLC

#elif defined(__MK20DX128__) && defined(TEENSYDUINO)
#define T3_0
#define HAS_TCK 20
#include "kinetis.h"

#elif defined(__MK20DX256__) && defined(TEENSYDUINO)
#define T3_2
#define HAS_TCK 20
#include "kinetis.h"

#elif defined(__MK64FX512__) && defined(TEENSYDUINO)
#define T3_5
#define HAS_TCK 20
#include "kinetis.h"

#elif defined(__MK66FX1M0__) && defined(TEENSYDUINO)
#define T3_6
#define HAS_TCK 20
#include "kinetis.h"

#elif defined(__IMXRT1062__) && defined(TEENSYDUINO)
#define T4_0
#define HAS_TCK 20
#include "imxrt.h"

#elif defined(ESP32)
//...

#else
#error "Board not supported"
#endif