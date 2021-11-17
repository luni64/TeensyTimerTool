#pragma once
//#include "Arduino.h"

#include "imxrt.h"
#include <cstdlib>

template <typename T>
class dmaBuffer_t
{
 public:
    dmaBuffer_t(size_t len)
    {
        bufSize   = len;
        allocated = malloc(bufSize * sizeof(T) + 31); // maybe change to unique_ptr?
        data      = (T *)(((uintptr_t)allocated + 31) & ~(uintptr_t)0x1F);
        //Serial.printf("alloc %p buf %p size: %d\n", allocated, data, bufSize);
    }

    ~dmaBuffer_t()
    {
        //Serial.printf("delete %p \n", allocated);
        free(allocated);
    }

    void flushCache()
    {
        arm_dcache_flush_delete(data, bufSize * sizeof(T));
    }

    size_t bufSize = 0;
    T *data        = nullptr;

 protected:
    void *allocated = nullptr;
};