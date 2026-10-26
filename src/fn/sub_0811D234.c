// COMPILER: old_agbcc
#include "global.h"
/* GameState_SetRuntimeFlag100: Sets runtime flag 0x100 at game-state offset 0x868; specific meaning unknown */
#define GameState_SetRuntimeFlag100 sub_0811D234

extern u8 *gGameState;

void sub_0811D234(void)
{
    u32 m = 0x100;
    *(u32 *)(gGameState + 0x868) |= m;
}
