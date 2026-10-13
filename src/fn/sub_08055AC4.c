#include "global.h"
extern u32 gUnk_02000498;
void Anim_Remove(u8 *);
s32 sub_08055AC4(u8 *p)
{
    s32 i = 0;
    if (i < p[0x19]) {
        u8 *q = p + 0x54;
        do {
            Anim_Remove(q);
            q += 0xbc;
            i++;
        } while (i < p[0x19]);
    }
    return gUnk_02000498 = 0;
}
