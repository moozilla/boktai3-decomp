#include "global.h"
#include "render.h"
struct Song {
    u32 p;
    u16 ms, me;
};
extern struct Song gSongTable[];
struct MP {
    u32 a, b, c;
};
extern struct MP gUnk_082643EC[];
extern u16 gUnk_03005470[];
void m4aSongNumStart(u16);
void Sound_PlaySong(u32 n)
{
    if ((gEnginePauseFlags & 6) == 0)
        gUnk_03005470[gSongTable[n].ms] = n;
    m4aSongNumStart(n);
}
