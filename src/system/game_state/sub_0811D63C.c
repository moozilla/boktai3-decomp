#include "engine/engine.h"

void sub_0811D600(u32, u32);
void GameState_RecordChallengeRankFromScript(void)
{
    u32 challenge = Script_GetKeywordValue('c', -1);
    u32 rank = Script_GetKeywordValue('r', -1);
    if (challenge <= 11 && rank <= 4)
        sub_0811D600(challenge, rank);
}
