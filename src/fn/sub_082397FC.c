#include "global.h"
#include "render.h"

struct P0813C94C {
    u8 f0[0x36A];
    u16 a;
    u16 b;
    u8 f36E[0xC];
    u16 c;
    u8 f37C[0xAC8 - 0x37C];
    u8 idx;
};

void LinkPlayer_LoadAnimationParameters(struct P0813C94C *p)
{
    u16 *t;
    s32 i;
    if (p->idx <= 2760) {
        t = (u16 *)(gPaletteBankData + 0x6A0);
        i = p->idx;
    } else {
        t = (u16 *)(gPaletteBankData + 0x2F60);
        i = p->idx;
        i -= 10;
    }
    t += i * 3;
    p->a = *t++;
    p->b = *t;
    p->c = t[1];
}
