#include "global.h"
struct Q {
    u8 f[0x8e8];
    u32 v;
};
extern struct Q *gUnk_020004F0[];
u32 Player_GetHorizontalVelocity(s32 i)
{
    if (i <= 1 && gUnk_020004F0[i] != 0)
        return gUnk_020004F0[i]->v;
    return 0;
}
