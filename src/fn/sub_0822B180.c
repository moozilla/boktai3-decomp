#include "global.h"
struct Song { u32 p; u16 ms, me; }; extern struct Song gSongTable[]; struct MP { u32 a, b, c; }; extern struct MP gUnk_082643EC[];
extern u16 gUnk_03005470[];
void m4aMPlayFadeOutTemporarily(u32, u16);
void sub_0822B180(u32 t) { u16 s = gUnk_03005470[0x18/2]; if (s != 0) { u32 m = gSongTable[s].ms; m4aMPlayFadeOutTemporarily(gUnk_082643EC[m].a, t); gUnk_03005470[m] = 0; } }
