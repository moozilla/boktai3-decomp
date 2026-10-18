#include "global.h"
s32 Script_GetKeywordValue(u32, s32);
void GameState_SetResultTimer(void);
void GameState_SetChallengeValueFromScript(void)
{
    s32 m = -1;
    if (Script_GetKeywordValue(0x63, m) >= 0 && Script_GetKeywordValue(0x74, m) >= 0)
        GameState_SetResultTimer();
}
