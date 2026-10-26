#define gMusicSlotPlayerTable gUnk_082643EC
#include "global.h"
#include "gba/m4a_internal.h"
#include "ram.h"

extern const struct MusicPlayer gMusicSlotPlayerTable[];
void m4aMPlayAllStop(void);

/* Tentative: preserve songs in resumable slots while stopping every player. */
void MusicSlots_StopAllAndRetainResumable(void)
{
    s32 i = 0;
    const struct MusicPlayer *players = gMusicSlotPlayerTable;
    u16 *slot = gMusicSlotSongs;
    u16 *songs = slot;
    u32 mask = 0xFFFF;
    u16 zero = i;
    do {
        if (i == 10) {
            u32 active = players[10].info->status & mask;
            if (active == 0)
                songs[10] = active;
        } else if (i != 12 && i != 29 && i != 30) {
            *slot = zero;
        }
        slot++;
        i++;
    } while (i <= 31);
    m4aMPlayAllStop();
}
