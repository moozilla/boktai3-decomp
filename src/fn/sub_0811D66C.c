#include "global.h"
u32 Script_GetKeywordValue(u32, s32);
void GameRecords_UpdateMinimum(u32);
void GameState_RecordChallengeScoreFromScript(void)
{
    s32 m = -1;
    u32 a = Script_GetKeywordValue(0x63, m);
    u32 b = Script_GetKeywordValue(0x74, m);
    if (a <= 11 && b <= 60000)
        GameRecords_UpdateMinimum(a);
}
