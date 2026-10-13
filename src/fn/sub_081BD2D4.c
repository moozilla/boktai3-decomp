#include "global.h"
void Anim_Remove(u8 *);
void Sprite_Remove(u8 *);
s32 sub_081BD2D4(u8 *p)
{
    u8 *q = p + 0x58;
    s32 i = 0x15;
    do {
        Anim_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    Sprite_Remove(p + 0x898);
    return 0;
}
