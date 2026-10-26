#include "global.h"
#include "ram.h"
void m4aSongNumStop(u16);
void Sound_SuspendPlayer29And30Songs(void)
{
    u16 *p = gMusicSlotSongs;
    if (p[0x3a / 2] != 0)
        m4aSongNumStop(p[0x3a / 2]);
    if (p[0x3c / 2] != 0)
        m4aSongNumStop(p[0x3c / 2]);
}
