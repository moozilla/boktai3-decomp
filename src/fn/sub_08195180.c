#include "global.h"

void sub_08195180(u8 *t)
{
    s32 x, z;
    {
        s32 *pos = (s32 *)(t + 0x72ac);
        s32 *vel = (s32 *)(t + 0x72b8);
        s32 v = *pos + *vel;
        *pos = v;
        if (*vel < 0) {
            if (v < -0x8000) {
                *pos = -0x8000;
                *vel = 0x88;
            }
        } else if (v > 0x8000) {
            *pos = 0x8000;
            *vel = -0x88;
        }
    }
    {
        s32 *pos = (s32 *)(t + 0x72b0);
        s32 *vel = (s32 *)(t + 0x72bc);
        s32 v = *pos + *vel;
        *pos = v;
        if (*vel < 0) {
            if (v < -0x5000)
                *vel = 0x400;
        } else if (v > 0x5000)
            *vel = -0x400;
    }
    x = *(s32 *)(t + 0x72a8) + *(s32 *)(t + 0x72ac);
    z = *(s32 *)(t + 0x72b0);
    {
        s16 v = (x >> 8) + *(u16 *)(t + 0x540c);
        *(s16 *)(t + 0x5404) = v;
        v = (z >> 8) + *(u16 *)(t + 0x5410);
        *(s16 *)(t + 0x5408) = v;
    }
}
