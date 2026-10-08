#include "global.h"

struct P0813FBE4 { u8 f0[0x34C]; u16 a[10]; };

void sub_0813FBE4(struct P0813FBE4 *p)
{
    s32 i;
    for (i = 0; i < 10; i++)
        p->a[i] |= 0xFFFF;
}
