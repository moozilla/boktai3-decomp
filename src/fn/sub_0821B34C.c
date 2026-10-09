#include "global.h"
struct Words { s32 elements[1]; };
struct Shorts { s16 elements[1]; };
void sub_0821B34C(u8 *p, s32 descriptor, s32 index, s32 value)
{
    switch ((descriptor >> 24) & 15) {
    case 9:
        ((struct Words *)p)->elements[index] = value;
        break;
    case 8: {
        u8 *q = p + index * 4;
        q[2] = value >> 16;
        q[1] = value >> 8;
        q[0] = value;
        break;
    }
    case 1: case 6:
        ((struct Shorts *)p)->elements[index] = value;
        break;
    case 2: case 3:
        p[index] = value;
        break;
    case 4: {
        s32 mask;
        index += (descriptor >> 16) & 15;
        p += index >> 3;
        mask = 1 << (index & 7);
        if (value)
            *p |= mask;
        else
            *p &= ~mask;
        break;
    }
    }
}
