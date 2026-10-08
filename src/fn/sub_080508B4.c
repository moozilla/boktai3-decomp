#include "global.h"
extern u8 *gUnk_0200048C;
extern u16 gUnk_02000528;
u32 sub_080508B4(void)
{
    u32 v = gUnk_02000528;
    u32 f = 0;
    if (v == 1) f = 1;
    if (f == 0) {
        if (gUnk_0200048C) return gUnk_0200048C[0x1F];
    }
    return 0;
}
