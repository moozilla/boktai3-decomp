#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sound_PauseChannel10(s32);
void Sound_PauseChannel10FromScript(void)
{
    if (Script_SeekToKeyword(0x66))
        Sound_PauseChannel10(Script_GetValue());
}
