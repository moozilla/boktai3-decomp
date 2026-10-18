#include "global.h"
extern u8 *gUnk_02000710;
u8 *GameState_GetConsumableDescriptor(s32);
u8 Inventory_GetConsumableCategory(u32 i)
{
    return GameState_GetConsumableDescriptor(*(s16 *)(gUnk_02000710 + i * 2 + 0x160))[0xc];
}
