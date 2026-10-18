#include "global.h"
void Script_GetKeywordValue(s32, s32);
void GameState_SetEventFlag(void);
void GameState_SetEventFlagFromScript(void)
{
    Script_GetKeywordValue(0x6b, -1);
    GameState_SetEventFlag();
}
