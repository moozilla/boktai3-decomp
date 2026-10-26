#include "global.h"
#include "engine.h"

extern u8 *gUnk_02000580;
extern u32 gUnk_02000560;

// differs: constant-vs-load order of the three flag tests
u8 Scene_CanOpenMenu(void)
{
    u8 *p = gUnk_02000580;
    u32 one;
    u32 mask;
    if (p && !(gEngineFlags & 0x200)) {
        one = 1;
        if ((gUnk_02000560 & one) && (u8)(p[0x418] - 3) > 2) {
            mask = 0x100000;
            if (!(*(u32 *)(p + 0x20) & mask) &&
                (!*(u16 *)(p + 0x538) || (u16)(*(u16 *)(p + 0x534) - 5) > 1)) {
                u8 v = p[0x457];
                u8 r = 1;
                if (v > 1 && v != 7)
                    r = 0;
                if (r)
                    return 1;
            }
        }
    }
    return 0;
}
