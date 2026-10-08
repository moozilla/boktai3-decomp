#include "global.h"

struct P0813F850 {
    u8 f0[0x33C]; s8 f33C; u8 f33D; u8 f33E;
    u8 f33F[0xBF5 - 0x33F]; u8 fBF5;
};
s32 sub_0813F808(void *);

s32 sub_0813F850(struct P0813F850 *p)
{
    if (p->f33E != 0 && sub_0813F808(p) != 0 && (u8)(p->fBF5 - 3) > 1)
        return p->f33C;
    return -1;
}
