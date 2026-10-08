#include "global.h"
struct E { u8 p[6]; u16 h; u8 q[24]; }; u32 sub_0822D6E0(void); u32 sub_0822DF38(u32); extern struct E gUnk_08E6092C[];
u32 sub_0822DFDC(u32 a) { s32 i = sub_0822D6E0(); if (i == 0xff) return 0; if (gUnk_08E6092C[i].h == 0) return 0; if (sub_0822DF38(a) != 0) return 0; return 1; }
