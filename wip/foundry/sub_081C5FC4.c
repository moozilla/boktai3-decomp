#include "global.h"

extern u16 gUnk_03005260[];
void TimeResult_BeginFadeOut(u8 *p);

void sub_081C5FC4(u8 *p)
{
    u16 *a;
    u16 *b;
    u16 held;

    held = gUnk_03005260[0];
    if (held & 0x20) {
        a = (u16 *)(p + 0x374);
        (*a)--;
    } else if (held & 0x10) {
        a = (u16 *)(p + 0x374);
        (*a)++;
    }

    held = gUnk_03005260[0];
    if (held & 0x40) {
        b = (u16 *)(p + 0x376);
        (*b)--;
    } else if (held & 0x80) {
        b = (u16 *)(p + 0x376);
        (*b)++;
    }

    if (gUnk_03005260[1] & 1)
        TimeResult_BeginFadeOut(p);
}
