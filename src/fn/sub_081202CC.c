#include "global.h"
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
void sub_08020CD4(u8 *, u32, u32);
void sub_081202CC(u8 *p)
{
    u32 m = 0x800;
    if (((gUnk_030053F4 | gUnk_030053F0) & m) == 0)
        sub_08020CD4(p + 0x858, *(u32 *)(p + 0x8bc), 0xc);
}
