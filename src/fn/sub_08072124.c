#include "global.h"

extern u8 *gUnk_020004A8;
extern u32 gUnk_030053F4;
s32 sub_080703F0(u32);

static inline u8 Test(u32 v, u32 mask)
{
    if (v & mask)
        return TRUE;
    return FALSE;
}

static inline u8 TestP(u32 mask, u32 *p)
{
    if (*p & mask)
        return TRUE;
    return FALSE;
}

u32 sub_08072124(void)
{
    u32 id;
    if (gUnk_020004A8 == NULL || !TestP(0x2000, (u32 *)(gUnk_020004A8 + 0x2c)))
        return 1;
    if (Test(gUnk_030053F4, 0x4800))
        id = 10;
    else {
        if (sub_080703F0(10))
            return 0;
        id = 23;
    }
    if (sub_080703F0(id))
        return 0;
    return 1;
}
