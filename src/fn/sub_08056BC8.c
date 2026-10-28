#include "script_utility.h"
u32 ScriptUtility_GetKeys(void)
{
    if (Script_SeekToKeyword('p')) {
        u32 keys = gPadState[1];
        return keys;
    }
    if (Script_SeekToKeyword('s'))
        return gPadState[0];
    {
        u32 keys;
        if (Script_SeekToKeyword('r') == 0)
            keys = gPadState[1];
        else
            keys = gPadState[2];
        return keys;
    }
}
