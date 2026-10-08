#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
static inline u32 tst(u32 *a, u32 m) { return *a & m; }
u32 sub_0815ACF4(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p != 0 && tst((u32 *)(p + 0x20), 0x100000))
        goto one;
    return 0;
one:
    return 1;
}
