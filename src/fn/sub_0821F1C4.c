#include "global.h"

// MATCHING_CONCESSION: register-pinning

u16 PairTable_Lookup(u16 *table, u16 stride, s32 a, s32 b)
{
    s32 idx;
    if (a == b)
        return 0;
    if (a <= b) {
        idx = a * stride + b - ((a + 2) * (a + 1) >> 1);
    } else {
        idx = b * stride + a - ((b + 2) * (b + 1) >> 1);
    }
    {
        register s32 j asm("r1") = idx;
        asm("" : "+r"(j));
        return table[j];
    }
}
