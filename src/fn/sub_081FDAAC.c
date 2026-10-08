#include "global.h"
struct S { u8 pad[0x3a4]; s32 w3a4; };
extern u8 *gUnk_02000580;
u32 sub_081FDAAC(struct S *p)
{
    u32 r;
    if (gUnk_02000580[0x457] != 0x1a)
        p->w3a4 -= 0x4000;
    if (p->w3a4 > 0)
        r = 0;
    else {
        p->w3a4 = 0;
        r = 1;
    }
    return r;
}
