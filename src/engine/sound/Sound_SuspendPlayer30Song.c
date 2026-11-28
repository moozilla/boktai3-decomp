#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aSongNumStop(u16);

void Sound_SuspendPlayer30Song(void)
{
    if (gMusicSlotSongs[30] != 0)
        m4aSongNumStop(gMusicSlotSongs[30]);
}
