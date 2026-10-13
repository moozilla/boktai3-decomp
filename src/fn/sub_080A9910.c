#include "global.h"

void sub_080A9910(u8 *p, s32 limit, s32 scale)
{
    u8 *q = *(u8 **)(p + 0x3d0);
    s16 *pos = (s16 *)(q + 0x8d8);
    if (*pos >= limit) {
        *pos = limit;
        *(u8 *)(q + 0x8d4) = 0xff;
    } else if (*pos <= -limit) {
        *pos = -limit;
        *(u8 *)(q + 0x8d4) = 1;
    }
    *(s16 *)(q + 0x8d8) += (s8)*(u8 *)(q + 0x8d4) * scale;
}
