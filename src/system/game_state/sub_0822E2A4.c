#include "global.h"
extern u8 *gGameState;
u8 *GameState_GetConsumableDescriptor(s32);
void GameState_EquipConsumableSlot(u32 a)
{
    *(u16 *)(gGameState +
             GameState_GetConsumableDescriptor(*(s16 *)(gGameState + a * 2 + 0x160))[0xc] * 2 +
             0x58) = a;
}
