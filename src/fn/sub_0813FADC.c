#include "global.h"

struct P0813FADC { u8 f0[0x418]; u8 f418; };
struct G0813FADC { u8 f0[0x52]; s16 idx; u8 f54[0xC]; s16 f60; };
extern struct G0813FADC *gUnk_02000710;
extern s16 gUnk_02000554;

s32 sub_0813FADC(struct P0813FADC *p)
{
    if (p->f418 != 6)
        return *(s16 *)((u8 *)gUnk_02000710 + gUnk_02000710->idx * 2 + 0x88);
    gUnk_02000554 = 0;
    if (gUnk_02000710->f60 == 0)
        return 4;
    return 5;
}
