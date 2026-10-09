#include "global.h"
struct Shorts { s16 elements[1]; };
void sub_0821B20C(u8 *p, s32 descriptor, s32 index, s32 *value)
{
    switch ((descriptor >> 24) & 15) {
    case 9:
        p += index * 4;
        *value = (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
        break;
    case 8:
        p += index * 4;
        *value = (p[1] << 8) | p[0];
        break;
    case 1: case 6:
        *value = ((struct Shorts *)p)->elements[index];
        break;
    case 2: case 3:
        p += index;
        *value = *p;
        break;
    case 4: {
        u8 *byte;
        u32 mask, bits;
        index += (descriptor >> 16) & 15;
        byte = p + (index >> 3);
        mask = 1u << (index & 7);
        bits = *byte & mask;
        *value = bits != 0;
        break;
    }
    }
}
