#include "global.h"
struct Song { u32 p; u16 ms, me; }; extern struct Song gSongTable[]; struct MP { u32 a, b, c; }; extern struct MP gUnk_082643EC[];
extern u16 gUnk_03005470[];
void m4aMPlayTempoControl(u32, u16);
void sub_0822B248(u32 t) { u16 s = gUnk_03005470[0x14/2]; if (s != 0) { u32 m = gSongTable[s].ms; m4aMPlayTempoControl(gUnk_082643EC[m].a, t); } }
