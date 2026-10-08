#include "global.h"

struct S { u8 filler[0x18]; u16 v; };
extern struct S *gUnk_02000110;

s32 sub_0804FFDC(struct S *p)
{
    p->v = 0;
    gUnk_02000110 = p;
    return 0;
}
