#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sound_StopSong(s32);
void sub_0822B3A4(void)
{
    if (Script_SeekToKeyword(0x69))
        Sound_StopSong(Script_GetValue());
}
