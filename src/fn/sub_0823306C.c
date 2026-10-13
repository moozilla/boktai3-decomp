#include "global.h"
void Sprite_Remove(void *);
extern u32 gUnk_020000AC;
u32 sub_0823306C(u8 *s)
{
    u8 *p = s + 0x18;
    s32 i = 0xf;
    do {
        if (p[8] != 0)
            Sprite_Remove(p + 4);
        i--;
        p += 0x4c;
    } while (i >= 0);
    return gUnk_020000AC = 0;
}
