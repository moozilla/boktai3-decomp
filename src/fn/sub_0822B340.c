#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sound_StartSongDirect(s32);
void Sound_StartSongDirectFromScript(void)
{
    if (Script_SeekToKeyword(0x69))
        Sound_StartSongDirect(Script_GetValue());
}
