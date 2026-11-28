#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aSongNumStop(u16);

void Sound_StopTrackedSong(u32 song)
{
    if (song) {
        u16 *slots = gMusicSlotSongs;
        u16 *slot = &slots[gSongTable[song].ms];
        if (song == *slot) {
            m4aSongNumStop(song);
            *slot = 0;
        }
    } else if (gMusicSlotSongs[10]) {
        m4aSongNumStop(gMusicSlotSongs[10]);
        gMusicSlotSongs[10] = 0;
    }
}
