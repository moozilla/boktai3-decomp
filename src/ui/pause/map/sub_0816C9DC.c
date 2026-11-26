#include "global.h"
extern u8 *gGameState;
u32 GameState_GetWorldMapFlag(u32 n)
{
    return *(u32 *)(gGameState + 0x5bc) & (1 << n);
}
