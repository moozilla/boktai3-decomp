#include "global.h"
#include "gba/m4a_internal.h"
#include "ram.h"

void sub_0822B4B8(void)
{
    s32 i;
    for (i = 0; i <= 31; i++) {
        m4aMPlayStop(gMPlayTable[i].info);
        gMusicSlotSongs[i] = 0;
    }
    m4aSoundInit();
}
