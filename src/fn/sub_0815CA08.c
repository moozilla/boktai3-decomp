#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 ScriptActor_GetCueOptionFromScript(void)
{
    if (Script_SeekToKeyword(0x65))
        return Script_GetValue();
    return 0;
}
