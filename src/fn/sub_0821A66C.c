#include "global.h"
u8 *sub_0821A66C(u8 *p, u32 *length)
{
    s32 size = *p & 15;
    switch (size) {
    case 13:
        *length = p[1];
        return p + 2;
    case 14: {
        u32 high = p[2] << 8;
        *length = p[1] | high;
        return p + 3;
    }
    case 15: {
        u8 *q = p + 1;
        *length = (q[2] << 16) | (q[1] << 8) | p[1];
        return p + 4;
    }
    default:
        *length = size;
        return p + 1;
    }
}
