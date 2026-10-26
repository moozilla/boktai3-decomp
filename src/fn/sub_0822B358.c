#include "global.h"
#include "render.h"
#include "ram.h"

struct SongEntry {
    void *header;
    u16 ms; /* music player */
    u16 me;
};
extern const struct SongEntry gSongTable[];
void m4aSongNumStop(u16);

/* Stops song. With sound off (engine flags bits 1-2) it is stopped outright;
   otherwise only when it is the song recorded for its player, whose record
   is cleared. */
void Sound_StopSong(u32 song)
{
    u32 off = gEnginePauseFlags & 6;
    if (off) {
        m4aSongNumStop(song);
    } else {
        u16 *players = gMusicSlotSongs;
        u16 *playing = &players[gSongTable[song].ms];
        if (song == *playing) {
            *playing = off;
            m4aSongNumStop(song);
        }
    }
}
