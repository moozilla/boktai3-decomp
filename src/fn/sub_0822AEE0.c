#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Sound_PlayTrackedSong(s32);
void Sound_PlayTrackedSongFromScript(void)
{
    if (Script_SeekToKeyword(0x69))
        Sound_PlayTrackedSong(Script_GetValue());
}
