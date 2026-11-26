#include "global.h"

void Anim_SetDurationScale(u8 *p, u16 v)
{
    if (*(u16 *)(p + 0xc) != v) {
        p[7] = (p[6] * v) >> 6;
        if (p[7] == 0)
            p[7] = 1;
        *(u16 *)(p + 0xc) = v;
    }
}
