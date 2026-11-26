// COMPILER: old_agbcc
#include "global.h"
#include "player/player.h"
/* Player_GetBasePosition: Returns the position block at offset 0x24 for the indexed player */

u8 *Player_GetBasePosition(s32 i)
{
    return (u8 *)&gPlayers[i]->positionalEffectGroup;
}
