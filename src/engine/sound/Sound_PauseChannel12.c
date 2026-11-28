#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"

void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo *, u16);

void Sound_PauseChannel12(u32 speed)
{
    u16 song = gMusicSlotSongs[12];
    if (song != 0) {
        u32 player = gSongTable[song].ms;
        m4aMPlayFadeOutTemporarily(gMPlayTable[player].info, speed);
        gMusicSlotSongs[player] = 0;
    }
}
