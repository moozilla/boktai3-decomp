#include "global.h"
#include "gba/m4a_internal.h"
#include "ram.h"
extern const struct MusicPlayer gUnk_082643EC[];
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Sound_SetPlayer10VolumeFromScript(void)
{
    if (Script_SeekToKeyword('v')) {
        s32 volume = Script_GetValue();
        s32 track;
        if (Script_SeekToKeyword('t'))
            track = Script_GetValue();
        else
            track = 255;
        {
            u16 *active = gMusicSlotSongs;
            u32 song = active[10];
            if (song) {
                u32 index = gSongTable[song].ms;
                m4aMPlayVolumeControl(gUnk_082643EC[index].info, (u16)track, (u16)volume);
            }
        }
    }
}
