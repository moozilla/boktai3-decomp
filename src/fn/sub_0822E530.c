#include "global.h"

#include "engine.h"
s32 GameState_HasEquipment(s32); /* unlock mask & (1 << id) */
/* Script: whether equipment -m is unlocked (bit -m of game state +0x4C);
   nothing is returned without -m. */
s32 GameState_HasEquipmentFromScript(void)
{
    if (Script_SeekToKeyword('m') != 0)
        return GameState_HasEquipment(Script_GetValue());
}
