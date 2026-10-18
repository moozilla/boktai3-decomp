#include "global.h"
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
s32 Nidhoggr_ChangeCompletedPartState(void *, s32, s32);
void Nidhoggr_SelectRandomPart22Animation(u8 *p)
{
    u8 *c;
    u16 t;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    t = gUnk_086141BC[gUnk_03005304];
    c = p + 0x7253;
    *c = (t & 3) + (1 + *c);
    if (*c > 5)
        *c = 0;
    if (p[0x724B] != 0)
        Nidhoggr_ChangeCompletedPartState(p, 0x16, *c + 0x3f);
    else
        Nidhoggr_ChangeCompletedPartState(p, 0x16, *c + 0x16);
}
