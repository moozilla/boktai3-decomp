#include "engine/shared/global.h"
#include "engine/shared/ram.h"
void m4aSongNumStart(u16);
void Sound_RestartPlayer30Song(void)
{
    if (gMusicSlotSongs[0x3c / 2] != 0)
        m4aSongNumStart(gMusicSlotSongs[0x3c / 2]);
}
