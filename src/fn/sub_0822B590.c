#include "global.h"
#include "ram.h"
struct Song {
    u32 p;
    u16 ms, me;
};
extern struct Song gSongTable[];
struct MP {
    u32 a, b, c;
};
extern struct MP gUnk_082643EC[];
struct T {
    s32 a;
    s32 b;
};
u32 Sound_IsSongPlaying(u32 n)
{
    u32 m = gSongTable[n].ms;
    if (((struct T *)gUnk_082643EC[m].a)->b >= 0 && gMusicSlotSongs[m] == n)
        return 1;
    return 0;
}
