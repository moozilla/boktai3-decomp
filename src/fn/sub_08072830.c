#include "global.h"
u32 sub_0821ABA8(u32, u32);
extern u8 *gUnk_020004A8;
void sub_08072830(void)
{
    u8 *p = gUnk_020004A8;
    if (p != 0)
        p[0x30] = sub_0821ABA8(0x6c, 3);
}
