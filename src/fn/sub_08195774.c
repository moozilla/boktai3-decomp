#include "global.h"
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
s32 sub_08249350(s32, s32);
void sub_08195774(u8 *p)
{
    const u16 *tbl;
    s32 i;
    u8 *q;
    u8 *a;
    p[0x725E] = 0;
    tbl = gUnk_086141BC;
    i = 8;
    q = p + 0x7267;
    do {
        *q = i;
        q--;
        i--;
    } while (i >= 0);
    i = 0;
    do {
        u8 t, *e;
        s32 r;
        gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
        r = sub_08249350(tbl[gUnk_03005304], 9);
        a = p + 0x725F;
        e = a + i;
        t = *e;
        *e = a[r];
        a[r] = t;
        i++;
    } while (i <= 8);
}
