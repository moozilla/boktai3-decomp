#include "global.h"

extern u8 *gUnk_020000DC;
u32 sub_080321CC(u8 *);
u32 sub_08031DFC(void)
{
    u8 *p = gUnk_020000DC;
    if (p == 0) return 0;
    return sub_080321CC(p + 0x18);
}
