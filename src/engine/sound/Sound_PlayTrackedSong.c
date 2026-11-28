#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aSongNumStartOrContinue(u16);
void m4aSongNumStart(u16);

void Sound_PlayTrackedSong(u32 song)
{
    const struct MusicPlayer *players;
    u32 player;

    if (gSongTable[song].ms == 10)
        m4aSongNumStartOrContinue(song);
    else
        m4aSongNumStart(song);
    players = gMPlayTable;
    player = gSongTable[song].ms;
    m4aMPlayVolumeControl(players[player].info, 0xffff, 256);
    gMusicSlotSongs[player] = song;
}
