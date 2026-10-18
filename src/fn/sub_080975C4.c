#include "global.h"
void Bee_UpdateSway(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3D0);
    s16 *s = (s16 *)(q + 0x5B8);
    s32 v = *s;
    if (v > 0x140) {
        *(u8 *)(q + 0x5B7) = 0xff;
        *s = 0x140;
    } else if (v < -0x140) {
        *(u8 *)(q + 0x5B7) = 1;
        *s = -0x140;
    }
    *(u16 *)(q + 0x5B8) += *(s8 *)(q + 0x5B8 - 1) * 12;
}
