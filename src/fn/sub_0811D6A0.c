#include "engine.h"

void GameState_SetResultTimer(s32 value);
void GameState_SetChallengeValueFromScript(void)
{
    if (Script_GetKeywordValue('c', -1) >= 0) {
        s32 timer = Script_GetKeywordValue('t', -1);
        if (timer >= 0)
            GameState_SetResultTimer(timer);
    }
}
