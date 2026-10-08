#include "global.h"

struct P { u32 fl; u8 f[0xba]; u16 a; u16 b; u16 c; u32 d; };

void sub_08107E90(s32 unused, struct P *p)
{
    p->fl |= 1;
    p->a = 3;
    p->d = 0;
    p->b &= 0xFFBD;
}
