#include "global.h"

struct P0813F7B4 { u8 f0[0x33c]; s8 f33C; u8 f33D[0x448 - 0x33D]; u32 f448; };
static inline u32 tst(u32 *a, u32 m) { return *a & m; }
s32 Div(s32, s32);
s32 sub_0813F784(s32);

s32 sub_0813F7B4(struct P0813F7B4 *p)
{
    s32 v;
    s32 pct;
    if (p->f33C < 0)
        return 0;
    v = sub_0813F784(p->f33C);
    pct = 100;
    if (p != 0 && tst(&p->f448, 0x1000))
        pct = 50;
    if (pct <= 99)
        v = Div(v * pct, 100);
    return v;
}
