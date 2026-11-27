#include "global.h"
extern u8 *gGameState;
void GameState_SetUnlockSlot(u32 i, u16 v)
{
    *(u16 *)(gGameState + i * 2 + 0x4c0) = v;
}
