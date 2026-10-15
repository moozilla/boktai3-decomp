#include "global.h"

struct S {
    u8 pad[0x90];
    u8 a[0xA8];
    u16 st;
    u16 pad2[2];
    u16 d;
};
void AnimData_SelectPaletteBank(void *, u32);

void sub_080640AC(struct S *s)
{
    if (s->d <= 5)
        AnimData_SelectPaletteBank(s->a, 0x192);
    else
        AnimData_SelectPaletteBank(s->a, 0x193);
    s->st = 2;
    s->d++;
}
