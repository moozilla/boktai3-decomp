#include "global.h"
void Bg_ClearTilemap(s32);
void sub_08112118(void *);
u32 sub_0811BB1C(void);
void Script_Call(u32, void *);
void sub_081123F0(u8 *s)
{
    u32 loc[3];
    u32 *p;
    u32 *q;
    u32 m;
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    sub_08112118(s);
    p = (u32 *)(s + 0x1210);
    if (*p != 0) {
        loc[0] = sub_0811BB1C();
        m = 0xFFFF0000;
        loc[1] = (loc[1] & m) | 1;
        q = &loc[1];
        q[1] = (u32)loc;
        Script_Call(*p, q);
    }
}
