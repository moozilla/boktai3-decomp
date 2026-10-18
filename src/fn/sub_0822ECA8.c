#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void GameState_InsertUniqueListEntry(s32);
s32 SolarFruit_AddFromScript(void)
{
    if (Script_SeekToKeyword(0x77)) {
        GameState_InsertUniqueListEntry(Script_GetValue());
        return 1;
    }
    return 0;
}
