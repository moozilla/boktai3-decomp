#include "global.h"
#include "ram.h"
void m4aSongNumStartOrContinue(u16);
void m4aSongNumStartOrChange(u16);
/* Restart the remembered songs of slots 10, 12, 29 and 30. */
void Sound_ResumePersistentSongs(void)
{
    s32 i;
    for (i = 0; i <= 31; i++) {
        if (gMusicSlotSongs[i] == 0)
            continue;
        if (i == 10)
            m4aSongNumStartOrContinue(gMusicSlotSongs[10]);
        else if (i == 12 || i == 29 || i == 30)
            m4aSongNumStartOrChange(gMusicSlotSongs[i]);
    }
}
