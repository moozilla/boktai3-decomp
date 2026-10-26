#include "global.h"
#include "ram.h"
void m4aSongNumStop(u16);
void Sound_SuspendPlayer30Song(void)
{
    if (gMusicSlotSongs[0x3c / 2] != 0)
        m4aSongNumStop(gMusicSlotSongs[0x3c / 2]);
}
