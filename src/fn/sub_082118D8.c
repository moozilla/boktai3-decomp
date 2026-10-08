#include "global.h"
struct G { u8 pad[0x720]; u32 a720; u32 a724; u32 a728; u32 a72c; u32 a730; };
extern struct G *gUnk_02000710;
u32 sub_082118D8(s32 n)
{
    u32 *p;
    if (n > 0x1f) {
        p = &gUnk_02000710->a724;
        n -= 0x20;
    } else {
        p = &gUnk_02000710->a720;
    }
    return *p & (1 << n);
}
