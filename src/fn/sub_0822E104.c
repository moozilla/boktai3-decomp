#include "global.h"
extern u8 *gUnk_02000710;
s32 Inventory_GetConsumableId(u32 i)
{
    return *(s16 *)(gUnk_02000710 + i * 2 + 0x160);
}
