#include "global.h"
s32 Script_GetKeywordValue(u32, s32);
void StateFlags_SetCategoryBit(s32, s32);
void GameFlag_SetFromScript(void)
{
    s32 m = -1;
    s32 a = Script_GetKeywordValue(0x6b, m);
    s32 b = Script_GetKeywordValue(0x69, m);
    if (a >= 0 && b >= 0)
        StateFlags_SetCategoryBit(a, b);
}
