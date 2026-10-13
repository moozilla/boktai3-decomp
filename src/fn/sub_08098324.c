#include "global.h"

void Sprite_Remove(u8 *);

void sub_08098324(u8 *p)
{
    u8 *q = p + 0x3d4;
    s32 i = 3;
    do {
        Sprite_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
}
