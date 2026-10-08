#include "global.h"
struct Song { u32 p; u16 ms, me; }; extern struct Song gSongTable[]; struct MP { u32 a, b, c; }; extern struct MP gUnk_082643EC[];
extern u16 gUnk_03005470[];
extern u32 gUnk_0300523C; void m4aSongNumStart(u16);
void sub_0822B2F8(u32 n) { if ((gUnk_0300523C & 6) == 0) gUnk_03005470[gSongTable[n].ms] = n; m4aSongNumStart(n); }
