#include "global.h"
void Anim_Remove(void *);
void sub_08112144(u8 *p)
{
    u8 *q = p + 0x24;
    s32 i = 0x16;
    do {
        Anim_Remove(q);
        q += 0x60;
    } while (--i >= 0);
}
