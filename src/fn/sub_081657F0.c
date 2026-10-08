#include "global.h"
struct B { u8 f[0xc]; u8 a; u8 b; };
extern struct B gUnk_03005430;
void sub_08165488(s32, s32, s32, s32, s32);
void sub_081657F0(s32 x, s32 y)
{
    sub_08165488(gUnk_03005430.a, 10, x, y, 0);
    sub_08165488(gUnk_03005430.b, 10, x + 3, y, 0);
}
