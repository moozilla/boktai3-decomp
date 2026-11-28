#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aSongNumStartOrContinue(u16);

void Sound_PlayChannel12Song(u32 song)
{
    if (gSongTable[song].ms == 12) {
        u32 player;
        m4aSongNumStartOrContinue(song);
        m4aMPlayVolumeControl(gMPlayTable[player = gSongTable[song].ms].info, 0xffff, 256);
        gMusicSlotSongs[player] = song;
    }
}
