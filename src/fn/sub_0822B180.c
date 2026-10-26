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
void m4aMPlayFadeOutTemporarily(u32, u16);
void Sound_PauseChannel12(u32 t)
{
    u16 s = gMusicSlotSongs[0x18 / 2];
    if (s != 0) {
        u32 m = gSongTable[s].ms;
        m4aMPlayFadeOutTemporarily(gUnk_082643EC[m].a, t);
        gMusicSlotSongs[m] = 0;
    }
}
