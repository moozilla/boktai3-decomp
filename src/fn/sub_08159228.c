#include "global.h"
/* Player_ClearSlotTimer: Clears the game-state timer entry selected by the player's slot index */
#define Player_ClearSlotTimer sub_08159228

extern u8 *gUnk_02000710;

void sub_08159228(u8 *p)
{
    *(u32 *)(gUnk_02000710 + p[0x472] * 4 + 0x628) = 0;
}
