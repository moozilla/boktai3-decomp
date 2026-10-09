#include "global.h"

extern u32 gUnk_02000488;
extern u8 *gUnk_02000580;
s32 sub_0805B28C(void *, void *, s32);

s32 sub_0805B2CC(u8 *a) {
    u8 *p;
    if (gUnk_02000488 == 0) goto fail;
    p = gUnk_02000580;
    if (*(u32 *)(p + 0x1C) != 1) goto fail;
    if (p[0x457] != 6) goto fail;
    if (sub_0805B28C((u8 *)gUnk_02000488 + 0x50, a + 0x510, 0x46) == 0) goto fail;
    return 1;
fail:
    return 0;
}
