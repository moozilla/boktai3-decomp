// COMPILER: old_agbcc
#include "global.h"
#include "system/game_state/game_state.h"
/* GameState_ClearEquipmentSlot: Marks one of the four equipment slots at offset 0x88 empty */

void GameState_ClearEquipmentSlot(s32 i)
{
    *(i + gGameState->slots88) = -1;
}
