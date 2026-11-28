#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"

void m4aMPlayAllStop(void);

/* Tentative: preserve songs in resumable slots while stopping every player. */
void MusicSlots_StopAllAndRetainResumable(void)
{
    s32 i;

    for (i = 0; i < 32; i++) {
        if (i == 10) {
            if ((gMPlayTable[10].info->status & 0xffff) == 0)
                gMusicSlotSongs[10] = 0;
        } else if (i != 12 && i != 29 && i != 30) {
            gMusicSlotSongs[i] = 0;
        }
    }
    m4aMPlayAllStop();
}
