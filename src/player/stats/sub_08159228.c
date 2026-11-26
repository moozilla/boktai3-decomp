#include "global.h"
/* Player_ClearSlotTimer: Clears the game-state timer entry selected by the player's slot index */

extern u8 *gGameState;

void Player_ClearSlotTimer(u8 *p)
{
    *(u32 *)(gGameState + p[0x472] * 4 + 0x628) = 0;
}
