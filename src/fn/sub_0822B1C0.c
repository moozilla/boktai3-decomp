#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sound_PauseChannel12(s32);
void Sound_PauseChannel12FromScript(void)
{
    if (Script_SeekToKeyword(0x66))
        Sound_PauseChannel12(Script_GetValue());
}
