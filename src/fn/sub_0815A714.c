#include "global.h"
u32 sub_0821ABA8(u32, u32);
extern u8 *gUnk_02000580;
void sub_0815A714(void)
{
    u8 *p = gUnk_02000580;
    if (p)
        *(u16 *)(p + 0x434) = sub_0821ABA8(0x70, 0);
}
