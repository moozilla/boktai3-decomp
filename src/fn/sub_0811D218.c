// COMPILER: old_agbcc
#include "global.h"
/* GameState_EnableAlternateHud: Sets flag 0x80 which selects the alternate HUD creator; tentative mode interpretation */
#define GameState_EnableAlternateHud sub_0811D218

extern u8 *gUnk_02000710;

void sub_0811D218(void)
{
    u32 m = 0x80;
    *(u32 *)(gUnk_02000710 + 0x868) |= m;
}
