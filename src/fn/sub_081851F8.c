#include "global.h"
struct P { u8 f2[0xB3C]; u8 a; u8 b; u8 c; };
s32 sub_081851F8(struct P *p, s32 x)
{
    if (p->a == x && p->b != 0xff) return 0;
    p->a = x;
    p->b = 0;
    p->c = 0;
    return 1;
}
