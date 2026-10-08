#include "global.h"

extern u16 gUnk_0300521C, gUnk_03005234;
extern u8 gUnk_03005230, gUnk_03005218;

void sub_082190E0(u16 a, u8 b, u8 c)
{
    gUnk_0300521C = 1;
    gUnk_03005234 = a;
    gUnk_03005230 = b;
    gUnk_03005218 = c;
}
