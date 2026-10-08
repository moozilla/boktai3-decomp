#include "global.h"

struct P0813C94C { u8 f0[0x36A]; u16 a; u16 b; u8 f36E[0xC]; u16 c; u8 f37C[0xAC8 - 0x37C]; u8 idx; };
extern u8 *gUnk_030042E4;

void sub_0813C94C(struct P0813C94C *p)
{
    u16 *t;
    s32 i;
    if (p->idx <= 9) {
        t = (u16 *)(gUnk_030042E4 + 0x6A0);
        i = p->idx;
    } else {
        t = (u16 *)(gUnk_030042E4 + 0x2F60);
        i = p->idx;
        i -= 10;
    }
    t += i * 3;
    p->a = *t++;
    p->b = *t;
    p->c = t[1];
}
