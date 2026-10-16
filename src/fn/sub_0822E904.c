#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void GameState_AddUnlock(s32);
s32 sub_0822E904(void)
{
    if (Script_SeekToKeyword(0x77)) {
        GameState_AddUnlock(Script_GetValue());
        return 1;
    }
    return 0;
}
