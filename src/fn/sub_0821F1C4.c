#include "global.h"

u16 PairTable_Lookup(u16 *table, u16 stride, s32 a, s32 b)
{
    if (a == b)
        return 0;
    if (a > b)
        return table[b * stride + a - ((b + 2) * (b + 1) >> 1)];
    return table[a * stride + b - ((a + 2) * (a + 1) >> 1)];
}
