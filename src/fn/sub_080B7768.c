#include "global.h"

void sub_080B7768(u8 *actor)
{
    u8 *p = *(u8 **)(actor + 0x3D0);
    s16 *t;
    t = (s16 *)(p + 0x6F0);
    if (*t > 0)
        (*t)--;
    t = (s16 *)(p + 0x6F2);
    if (*t > 0)
        (*t)--;
    t = (s16 *)(p + 0x6F4);
    if (*t > 0)
        (*t)--;
    t = (s16 *)(p + 0x6F6);
    if (*t > 0)
        (*t)--;
}
