#include "global.h"
void Script_GetKeywordValue(s32, s32);
void GameState_SetEventFlag(void);
void sub_081D5260(void)
{
    Script_GetKeywordValue(0x6b, -1);
    GameState_SetEventFlag();
}
