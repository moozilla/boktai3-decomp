#include "engine.h"

s32 GameRecords_UpdateMinimum(u32 index, s32 value);
void GameState_RecordChallengeScoreFromScript(void)
{
    u32 challenge = Script_GetKeywordValue('c', -1);
    u32 score = Script_GetKeywordValue('t', -1);
    if (challenge <= 11 && score <= 60000)
        GameRecords_UpdateMinimum(challenge, score);
}
