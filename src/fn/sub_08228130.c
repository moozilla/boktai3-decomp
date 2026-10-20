#include "global.h"

extern u8 *gUnk_02000580;
extern u32 gUnk_030053F4;
extern u32 gUnk_02000560;

#ifdef NONMATCHING
// fuzzy: 97.2%
// differs: constant-vs-load order of the three flag tests
u8 Scene_CanOpenMenu(void)
{
    u8 *p = gUnk_02000580;
    if (p && !(gUnk_030053F4 & 0x200) && (gUnk_02000560 & 1) && (u8)(p[0x418] - 3) > 2 &&
        !(*(u32 *)(p + 0x20) & 0x100000) &&
        (!*(u16 *)(p + 0x538) || (u16)(*(u16 *)(p + 0x534) - 5) > 1)) {
        u8 v = p[0x457];
        u8 r = 1;
        if (v > 1 && v != 7)
            r = 0;
        if (r)
            return 1;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Scene_CanOpenMenu);
#endif
