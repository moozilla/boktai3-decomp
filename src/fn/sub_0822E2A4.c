#include "global.h"
extern u8 *gUnk_02000710;
u8 *GameState_GetConsumableDescriptor(s32);
void GameState_EquipConsumableSlot(u32 a)
{
    *(u16 *)(gUnk_02000710 +
             GameState_GetConsumableDescriptor(*(s16 *)(gUnk_02000710 + a * 2 + 0x160))[0xc] * 2 +
             0x58) = a;
}
