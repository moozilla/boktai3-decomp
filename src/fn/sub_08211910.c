#include "global.h"
struct G { u8 pad[0x720]; u32 a720; u32 a724; u32 a728; u32 a72c; u32 a730; };
extern struct G *gUnk_02000710;
u32 sub_08211910(s32 n)
{
    u32 a = gUnk_02000710->a720 | 0x04000400;
    u32 b = gUnk_02000710->a724 | 0x8020;
    if (n <= 0x1f)
        return a & (1 << n);
    return b & (1 << (n - 0x20));
}
