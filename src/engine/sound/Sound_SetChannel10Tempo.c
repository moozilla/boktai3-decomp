#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"

void Sound_SetChannel10Tempo(u32 tempo)
{
    u16 song = gMusicSlotSongs[10];
    if (song != 0) {
        u32 player = gSongTable[song].ms;
        m4aMPlayTempoControl(gMPlayTable[player].info, tempo);
    }
}
