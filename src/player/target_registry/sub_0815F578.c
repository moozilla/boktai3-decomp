// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/player.h"
/* Player_GetBasePosition: Returns the position block at offset 0x24 for the indexed player */
#define Player_GetBasePosition sub_0815F578

u8 *sub_0815F578(s32 i)
{
    return (u8 *)&gPlayers[i]->positionalEffectGroup;
}
