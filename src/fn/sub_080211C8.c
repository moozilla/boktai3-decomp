#include "global.h"
struct P { u8 pad[8]; s16 a; s16 b; s16 c; };
void sub_080037B0(s32, s32, s32);
void sub_080211C8(u32 x, u32 y, struct P *p)
{
    s32 c;
    s32 m;
    s32 t = p->c;
    c = 0x7FFF;
    if (t == 0) c = 0x1084;
    switch (p->b) {
    case 0: m = 0xFFF; break;
    case 1: m = 0x1000; break;
    case 2: m = 0xE000; break;
    case 3: m = 0x1FFF; break;
    case 4:
    default: m = 0xFFFF; break;
    }
    sub_080037B0(p->a, m, c);
}
