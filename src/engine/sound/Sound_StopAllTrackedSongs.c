#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"
void m4aSongNumStop(u16);

void Sound_StopAllTrackedSongs(void)
{
    s32 i;

    for (i = 0; i < 32; i++) {
        if (gMusicSlotSongs[i] != 0) {
            m4aSongNumStop(gMusicSlotSongs[i]);
            gMusicSlotSongs[i] = 0;
        }
    }
}
