#include "global.h"

extern u32 gUnk_02000000[];
struct S { u8 f[0x1b]; u8 i; };

u32 sub_08001AD0(struct S *p)
{
    gUnk_02000000[p->i] = 0;
    return 0;
}
