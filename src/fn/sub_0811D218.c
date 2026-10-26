// COMPILER: old_agbcc
#include "global.h"
/* GameState_EnableAlternateHud: Sets flag 0x80 which selects the alternate HUD creator; tentative mode interpretation */
#define GameState_EnableAlternateHud sub_0811D218

extern u8 *gGameState;

void sub_0811D218(void)
{
    u32 m = 0x80;
    *(u32 *)(gGameState + 0x868) |= m;
}
