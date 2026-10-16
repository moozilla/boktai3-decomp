#include "global.h"
#include "engine.h"
s32 sub_0822E618(s32 index);
/* Script: 1 if -w is one of the twelve entries read by sub_0822E618 (the
   list reset by GameState_ResetUnlocksFromScript), else 0. */
s32 GameState_HasUnlockFromScript(void)
{
    s32 value, i;
    if (!Script_SeekToKeyword('w'))
        return 0;
    value = Script_GetValue();
    for (i = 0; i <= 11; i++) {
        if (sub_0822E618(i) == value)
            return 1;
    }
    return 0;
}
