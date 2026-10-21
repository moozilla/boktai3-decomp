#include "global.h"

// MATCHING_CONCESSION: register-pinning
// Open: the final subtract writes r0 (idx in r0); ROM keeps the index in r1
// (subs r1, r1, r0) and loads through it. Triangular-index lookup of a
// symmetric pair table (a != b), stride-major.

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
