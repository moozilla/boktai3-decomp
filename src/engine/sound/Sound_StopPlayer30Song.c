#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void Sound_StopSong(u16);

void Sound_StopPlayer30Song(void)
{
    if (gMusicSlotSongs[30] != 0)
        Sound_StopSong(gMusicSlotSongs[30]);
}
