#include "global.h"
extern void *gUnk_02000480;
u32 sub_0821ABA8(u32, u32);
void sub_081C6954(void *, u32);
void sub_081C6B84(void)
{
    void *p = gUnk_02000480;
    if (p != 0) sub_081C6954(p, sub_0821ABA8(0x6e, 0));
}
