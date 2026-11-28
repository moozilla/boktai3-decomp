#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aSongNumStart(u16);

void Sound_RestartPlayer30Song(void)
{
    if (gMusicSlotSongs[30] != 0)
        m4aSongNumStart(gMusicSlotSongs[30]);
}
