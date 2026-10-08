#include "global.h"
extern u8 *gUnk_020003C8;
u8 *sub_081EA730(u32, u32);
u8 sub_081E5560(void)
{
    u8 *p = gUnk_020003C8;
    if (p == 0) {
        p = sub_081EA730(0xBB34, 0);
        if (p == 0) return 0;
    }
    return p[0xF68];
}
