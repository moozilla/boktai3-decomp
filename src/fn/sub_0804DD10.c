#include "global.h"
void Sprite_Remove(u8 *);
void sub_08020D08(u8 *);
u32 sub_0804DD10(u8 *p)
{
    u8 *q = p + 0x74;
    s32 i = 4;
    do {
        Sprite_Remove(q);
        q += 0x2C;
        i--;
    } while (i >= 0);
    if (*(u16 *)(p + 0x4C) != 0)
        sub_08020D08(p + 0x18);
    return 0;
}
