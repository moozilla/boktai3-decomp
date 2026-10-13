#include "global.h"
void Anim_Remove(u8 *);
void sub_0801A8CC(u8 *p)
{
    u8 *e = p + 0x674;
    s32 i = 0x12;
    do {
        Anim_Remove(e);
        e += 0x60;
        i--;
    } while (i >= 0);
    Anim_Remove(p + 0x614);
}
