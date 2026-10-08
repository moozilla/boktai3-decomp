#include "global.h"

struct S30042A0 { u8 pad[0x12]; u16 f12; };
extern struct S30042A0 gUnk_030042A0;
extern u32 gUnk_030042B4;
extern u32 gUnk_03004294;
extern u32 gUnk_030042B8;

void sub_0821447C(u16 a, u32 b, u32 c, u32 d)
{
    gUnk_030042A0.f12 = a;
    gUnk_030042B4 = b;
    gUnk_03004294 = c;
    gUnk_030042B8 = d;
}
