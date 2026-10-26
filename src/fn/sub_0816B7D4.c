#include "global.h"
extern u8 *gGameState;
u32 GameState_GetMapRevealFlag(u32 n)
{
    return *(u32 *)(gGameState + 0x5b8) & (1 << n);
}
