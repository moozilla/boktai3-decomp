#include "global.h"
extern u16 gUnk_020005A0;
u32 Inventory_GetUsePermissionFlag(u32 bit)
{
    return gUnk_020005A0 & (1 << bit);
}
