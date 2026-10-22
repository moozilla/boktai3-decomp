#include "global.h"
void sub_08219E44(void *dest, u32 value, s32 size)
{
    u32 fill;
    s32 words;
    u8 *dst = dest;
    value &= 0xff;
    while (size > 0 && ((u32)dst & 3)) {
        *dst++ = value;
        size--;
    }
    value = (value << 8) | value | (value << 16) | (value << 24);
    fill = value;
    words = size & ~3;
    CpuSet(&fill, dst, 0x05000000 | ((words / 4) & 0x1fffff));
    dst += words;
    size &= 3;
    while (size > 0) {
        *dst = value;
        size--;
        dst++;
    }
}
