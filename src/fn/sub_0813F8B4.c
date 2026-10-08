#include "global.h"

struct P0813F8B4 { u8 f0[0x472]; u8 idx; };
struct G0813F8B4 { u8 f0[0x876]; s16 f876; };
extern struct G0813F8B4 *gUnk_02000710;

s32 sub_0813F8B4(struct P0813F8B4 *p, s32 v)
{
    if (*(s32 *)((u8 *)gUnk_02000710 + p->idx * 4 + 0x628) <= 0 && gUnk_02000710->f876 >= v)
        return 1;
    return 0;
}
