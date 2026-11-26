// COMPILER: old_agbcc
#include "global.h"
/* GameState_ClearEquipmentSlot: Marks one of the four equipment slots at offset 0x88 empty */

extern u8 *gGameState;

void GameState_ClearEquipmentSlot(s32 i)
{
    *(s16 *)(gGameState + i * 2 + 0x88) = -1;
}
