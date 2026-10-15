#include "global.h"

struct P {
    u8 f00[0xb8];
    s32 st;
    u8 sub[1];
};

void AnimData_SelectPaletteBank(void *, s32);

void sub_08162DAC(struct P *p)
{
    if (p != 0) {
        p->st = 2;
        AnimData_SelectPaletteBank((u8 *)p + 0xbc, 0x65);
    }
}
