#include "global.h"
s32 Script_GetKeywordValue(u32, s32);
void GameState_SetChallengeCompletionFlag(void);
void GameState_SetChallengeCompletionFlagFromScript(void)
{
    if (Script_GetKeywordValue(0x63, -1) >= 0)
        GameState_SetChallengeCompletionFlag();
}
