#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/video/render.h"
#include "engine/ram.h"
void m4aSongNumStop(u16);

/* Stops song. With sound off (engine flags bits 1-2) it is stopped outright;
   otherwise only when it is the song recorded for its player, whose record
   is cleared. */
void Sound_StopSong(u32 song)
{
    if (gEnginePauseFlags & 6) {
        m4aSongNumStop(song);
    } else {
        u16 *slots = gMusicSlotSongs;
        u16 *playing = &slots[gSongTable[song].ms];
        if (song == *playing) {
            *playing = 0;
            m4aSongNumStop(song);
        }
    }
}
