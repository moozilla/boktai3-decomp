#include "global.h"
#include "gba/m4a_internal.h"
#include "engine/ram.h"

u32 Sound_IsSongPlaying(u32 n)
{
    u32 player = gSongTable[n].ms;
    if ((s32)gMPlayTable[player].info->status >= 0 && gMusicSlotSongs[player] == n)
        return 1;
    return 0;
}
