#define Math_MultiplyUnsignedHigh sub_0822F1B8
// COMPILER: agbcc_arm
// CFLAGS: -O2 -mthumb-interwork -fomit-frame-pointer
#include "global.h"
u32 sub_0822F1B8(u32 a, u32 b)
{
    return ((u64)a * b) >> 32;
}
