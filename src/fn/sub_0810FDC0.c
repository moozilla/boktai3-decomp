#include "global.h"

void Anim_Remove(u8 *);

void sub_0810FDC0(u8 *p)
{
    u8 *q = p + 0x24;
    s32 i;

    for (i = 0x11; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x60;
    }
}
