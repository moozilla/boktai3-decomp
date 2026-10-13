#include "global.h"
void Anim_Remove(u8 *);
void sub_0801A67C(u8 *p)
{
    u8 *e = p + 0x194;
    s32 i = 2;
    do {
        Anim_Remove(e);
        e += 0x60;
        i--;
    } while (i >= 0);
}
