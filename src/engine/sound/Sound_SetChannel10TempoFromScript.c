#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sound_SetChannel10Tempo(s32);
void Sound_SetChannel10TempoFromScript(void)
{
    if (Script_SeekToKeyword(0x74))
        Sound_SetChannel10Tempo(Script_GetValue());
}
