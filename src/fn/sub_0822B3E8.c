#include "global.h"
#include "ram.h"
void m4aSongNumStop(u16);
void Sound_StopAllTrackedSongs(void)
{
    u16 *p = gMusicSlotSongs;
    s32 i = 0x1f;
    do {
        if (*p != 0) {
            m4aSongNumStop(*p);
            *p = 0;
        }
        p++;
        i--;
    } while (i >= 0);
}
