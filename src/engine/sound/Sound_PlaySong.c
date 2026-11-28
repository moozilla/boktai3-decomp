#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/video/render.h"
#include "engine/ram.h"
void m4aSongNumStart(u16);

void Sound_PlaySong(u32 n)
{
    if ((gEnginePauseFlags & 6) == 0)
        gMusicSlotSongs[gSongTable[n].ms] = n;
    m4aSongNumStart(n);
}
