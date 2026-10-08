#include "global.h"
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
s32 sub_08190C98(void *, s32, s32);
void sub_08194C54(u8 *p)
{
    u8 *c;
    u16 t;
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    t = gUnk_086141BC[gUnk_03005304];
    c = p + 0x7254;
    *c = (t & 3) + (1 + *c);
    if (*c > 5) *c = 0;
    sub_08190C98(p, 0x17, *c + 0x2a);
}
