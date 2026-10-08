#include "global.h"

extern u32 gUnk_02000010[];
struct S { u8 f[0x1b]; u8 i; };

u32 sub_08002658(struct S *p)
{
    u32 *q = gUnk_02000010;
    u32 i = p->i;
    q[i] = 0;
}
