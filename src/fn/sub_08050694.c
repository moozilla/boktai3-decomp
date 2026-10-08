#include "global.h"
extern u8 *gUnk_02000580;
void sub_0824923C(u8 *, u32);
u32 sub_08050694(u8 *p)
{
    if (!(*(u32 *)(gUnk_02000580 + 0x1C) & 4))
        sub_0824923C(p, *(u32 *)(p + 0x38));
    return 0;
}
