#include "global.h"
void sub_08030498(u8 *a, u8 *p)
{
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 0xa;
    }
    if (p[7] != 0) {
        u32 c = 0xc;
        u32 z = 0;
        p[4] = c;
        *(u32 *)(p + 0x14) = z;
        p[8] = 1;
    }
    *(u32 *)(p + 0x14) += 1;
}
