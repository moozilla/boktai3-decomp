#include "global.h"
/* Player_ClearSlotTimer: Clears the game-state timer entry selected by the player's slot index */
#define Player_ClearSlotTimer sub_08159228

extern u8 *gGameState;

void sub_08159228(u8 *p)
{
    *(u32 *)(gGameState + p[0x472] * 4 + 0x628) = 0;
}
