#include "global.h"

s32 sub_0821ABA8(s32, s32);
u8 *sub_080FF820(void);

s32 sub_08100248(void)
{
    if (sub_0821ABA8(0x6e, 0) != 0) {
        u8 *p = sub_080FF820();
        if (p != 0)
            return *(u16 *)(p + 0xc8);
    }
    return -1;
}
