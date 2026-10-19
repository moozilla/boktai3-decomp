#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.7%
// foundry draft (worker best 80.0%); what differs is mostly register allocation and instruction order
extern const u32 gUnk_08605E48[8];
u32 sub_0809CC8C(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3D0);
    s32 i = *(u16 *)(q + 0x3E0) >> 7;
    if (i > 7)
        i = 7;
    return gUnk_08605E48[i];
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0809CC8C);
#endif
