#include "global.h"
extern u32 gUnk_02000078;
void Anim_Remove(u8 *);
u32 sub_08016EFC(u8 *p)
{
    s32 i;
    u8 *q = p + 0x60;
    for (i = 7; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x84;
    }
    {
        u32 z = 0;
        gUnk_02000078 = z;
    }
}
