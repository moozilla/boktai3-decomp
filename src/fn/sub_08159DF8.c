#include "global.h"

struct P08159DF8 { u8 f0[0x1c]; s32 f1c; u8 f20[0x457 - 0x20]; u8 f457; };
s32 sub_0813AFEC(void *, u32, u32);

s32 sub_08159DF8(struct P08159DF8 *p)
{
    if (p->f1c != 1 || p->f457 != 0x19)
        return -1;
    return sub_0813AFEC(p, 0, 0);
}
