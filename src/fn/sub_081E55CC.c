#include "global.h"
extern u8 *gUnk_020003C8;
u8 *sub_081EA730(u32, u32);
u8 sub_0821ABA8(u32, u32);
void sub_081E55CC(void)
{
    u8 *p = gUnk_020003C8;
    if (p == 0) {
        p = sub_081EA730(0xBB34, 0);
        if (p == 0) return;
    }
    p[0xF6C] = sub_0821ABA8(0x72, 0);
}
