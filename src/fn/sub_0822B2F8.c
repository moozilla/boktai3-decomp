#include "global.h"
#include "render.h"
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
void m4aSongNumStart(u16);
void Sound_PlaySong(u32 n)
{
    if ((gEnginePauseFlags & 6) == 0)
        gMusicSlotSongs[gSongTable[n].ms] = n;
    m4aSongNumStart(n);
}
