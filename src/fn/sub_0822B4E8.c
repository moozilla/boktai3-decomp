#include "global.h"
#include "ram.h"
void Sound_StopSong(u16);
void Sound_StopPlayer30Song(void)
{
    if (gMusicSlotSongs[0x3c / 2] != 0)
        Sound_StopSong(gMusicSlotSongs[0x3c / 2]);
}
