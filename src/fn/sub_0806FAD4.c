#include "global.h"
u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
extern u8 *gUnk_020004AC;
void sub_0806FAD4(u8 *p)
{
    u8 *q;
    *(u8 **)(p + 0x1c) = sub_08219C40(8);
    sub_08219DD8(*(u8 **)(p + 0x1c), 8);
    q = *(u8 **)(p + 0x1c);
    *(u32 *)q = 0;
    *(u32 *)(q + 4) = 0;
    gUnk_020004AC = q;
    *(u8 **)(p + 0x20) = q;
}
