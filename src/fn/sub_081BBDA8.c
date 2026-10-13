#include "global.h"
void Anim_Remove(u8 *);
void Sprite_Remove(u8 *);
s32 sub_081BBDA8(u8 *p)
{
    u8 *q = p + 0x58;
    s32 i = 0xf;
    do {
        Anim_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    q = p + 0x658;
    i = 1;
    do {
        Sprite_Remove(q);
        q += 0x2c;
        i--;
    } while (i >= 0);
    return 0;
}
