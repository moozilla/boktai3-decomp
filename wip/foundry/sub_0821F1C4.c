#define Navigation_GetPairValue sub_0821F1C4
#include "global.h"
u32 sub_0821F1C4(const u16 *table, u16 count, s32 a, s32 b)
{
    s32 x = a;
    s32 index = count;
    s32 upper = count;
    if (x == b)
        return 0;
    if (x <= b) {
        index *= x;
        index += b;
        upper = x + 2;
        x++;
    } else {
        index = b * upper;
        index += x;
        upper = b + 2;
        x = b + 1;
    }
    index -= (upper * x) >> 1;
    return *(const u16 *)((index << 1) + (u32)table);
}
