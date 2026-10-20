#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.5%
// Open: the final subtract writes r0 (idx in r0); ROM keeps the index in r1
// (subs r1, r1, r0) and loads through it. Triangular-index lookup of a
// symmetric pair table (a != b), stride-major.
#define PairTable_Lookup sub_0821F1C4

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
    return table[idx];
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821F1C4);
#endif
