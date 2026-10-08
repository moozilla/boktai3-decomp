#include "global.h"

extern u16 gUnk_03005214;
extern u16 gUnk_030051C4;
extern u32 gUnk_03004DA0;
extern u16 gUnk_030051DC;
extern u16 gUnk_03005210;
extern u32 gUnk_030051D4;

void sub_08163FA8(u32 a, u32 b)
{
    gUnk_03005214 = b;
    gUnk_030051C4 = 0xFFFF;
    gUnk_03004DA0 = a;
    gUnk_030051DC = b;
    gUnk_03005210 = 0;
    gUnk_030051D4 = a;
}
