#include "global.h"

struct P0813F850 {
    u8 f0[0x33C];
    s8 f33C;
    u8 f33D;
    u8 f33E;
    u8 f33F[0xBF5 - 0x33F];
    u8 fBF5;
};
s32 Player_CanAffordEnchantment(void *);

s32 Player_GetUsableEnchantment(struct P0813F850 *p)
{
    if (p->f33E != 0 && Player_CanAffordEnchantment(p) != 0 && (u8)(p->fBF5 - 3) > 1)
        return p->f33C;
    return -1;
}
