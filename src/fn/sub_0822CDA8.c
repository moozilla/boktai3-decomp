#include "global.h"
extern u8 *gUnk_02000710;
u32 Inventory_GetItemOverrideFlag(u32);
s32 Inventory_GetItemId(u32 i)
{
    s32 r;
    if (Inventory_GetItemOverrideFlag(i) == 0)
        r = *(s16 *)(gUnk_02000710 + i * 2 + 0xa0);
    else
        r = 9;
    return r;
}
