#include "global.h"
#include "engine.h"
s32 GameState_GetUnlockEntry(s32 index);
/* Script: 1 if -w is one of the twelve entries read by GameState_GetUnlockEntry (the
   list reset by GameState_ResetUnlocksFromScript), else 0. */
s32 GameState_HasUnlockFromScript(void)
{
    s32 value, i;
    if (!Script_SeekToKeyword('w'))
        return 0;
    value = Script_GetValue();
    for (i = 0; i <= 11; i++) {
        if (GameState_GetUnlockEntry(i) == value)
            return 1;
    }
    return 0;
}
