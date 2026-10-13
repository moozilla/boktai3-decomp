#include "global.h"
extern u32 gUnk_020000A8;
void Anim_Remove(u8 *);
u32 sub_08047BE8(u8 *p)
{
    u8 *q = p + 0x38;
    s32 i = 15;
    do {
        if (q[8] != 0)
            Anim_Remove(q + 4);
        i--;
        q += 0x64;
    } while (i >= 0);
    return gUnk_020000A8 = 0;
}
