#include "global.h"
extern u8 *gUnk_020005EC;
void sub_081FE6E0(void);
u8 *sub_081FEA58(s32 i)
{
    u8 *g = gUnk_020005EC;
    u8 *r;
    if (g != 0)
        r = (u8 *)(i * 0x3ac + (u32)g) + 0x88;
    else {
        sub_081FE6E0();
        r = 0;
    }
    return r;
}
