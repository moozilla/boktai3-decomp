#include "global.h"
extern u8 *gGameState;
void GameState_ClearChallengeCompletionFlags(void)
{
    *(u32 *)(gGameState + 0x634) = 0;
}
