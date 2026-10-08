#include "global.h"
struct Song { u32 p; u16 ms, me; }; extern struct Song gSongTable[]; struct MP { u32 a, b, c; }; extern struct MP gUnk_082643EC[];
extern u16 gUnk_03005470[];
struct T { s32 a; s32 b; };
u32 sub_0822B590(u32 n) { u32 m = gSongTable[n].ms; if (((struct T *)gUnk_082643EC[m].a)->b >= 0 && gUnk_03005470[m] == n) return 1; return 0; }
