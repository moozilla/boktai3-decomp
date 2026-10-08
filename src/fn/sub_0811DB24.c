#include "global.h"
struct G { u8 f[0x634]; u32 v; };
extern struct G *gUnk_02000710;
u32 sub_0811DB24(u32 i)
{
    u32 r;
    if (i <= 11) {
        gUnk_02000710->v |= 1 << i;
        r = 1;
    } else r = 0;
    return r;
}
