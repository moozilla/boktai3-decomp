#include "global.h"
#include "system/game_state/game_state.h"

u8 *GameState_GetConsumableDescriptor(s32);
void GameState_EquipConsumableSlot(u32 slot)
{
    *(GameState_GetConsumableDescriptor(*(slot + gGameState->table160))[0xc] + gGameState->party) =
        slot;
}
