#include "global.h"

#define GameState_HasEquipmentFromScript sub_0822E530
#include "engine.h"
s32 sub_0822E51C(s32); /* unlock mask & (1 << id) */
/* Script: whether equipment -m is unlocked (bit -m of game state +0x4C);
   nothing is returned without -m. */
s32 GameState_HasEquipmentFromScript(void)
{
    if (Script_SeekToKeyword('m') != 0)
        return sub_0822E51C(Script_GetValue());
}
