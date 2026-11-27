#include "global.h"
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void Sound_StopTrackedSong(u32);
void Sound_StopTrackedSongFromScript(void)
{
    u32 v;
    if (Script_SeekToKeyword(0x69))
        v = Script_GetValue();
    else
        v = 0;
    Sound_StopTrackedSong(v);
}
