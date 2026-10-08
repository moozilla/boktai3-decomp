#include "global.h"

struct P0814DF0C {
    u8 f0[0x428]; u16 f428;
    u8 f42A[0xBF5 - 0x42A]; u8 fBF5;
    u8 fBF6[0xC18 - 0xBF6]; u8 fC18;
};
struct G0814DF0C { u8 f0[0x77C]; s32 f77C; };
extern struct G0814DF0C *gUnk_02000710;

s32 sub_0814DF0C(struct P0814DF0C *p, s32 v)
{
    s32 a;
    if (p->fBF5 != 0 && *(u8 *)((u32)p + 0xC18) == 7)
        a = gUnk_02000710->f77C;
    else
        a = p->f428;
    if (a < v)
        return 1;
    return 0;
}
