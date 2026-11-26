#include "global.h"
extern u8 *gGameState;
void GameState_ClearConsumableSlot(u32 i)
{
    *(s16 *)(gGameState + i * 2 + 0x160) = -1;
}
