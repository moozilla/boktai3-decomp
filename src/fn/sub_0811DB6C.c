#include "global.h"
extern u8 *gUnk_02000710;
void GameState_ClearChallengeCompletionFlags(void)
{
    *(u32 *)(gUnk_02000710 + 0x634) = 0;
}
