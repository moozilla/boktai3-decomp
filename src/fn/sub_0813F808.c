#include "global.h"

s32 sub_0813F7B4(u8 *p);

s32 sub_0813F808(u8 *p)
{
    u8 *q = p;
    s32 r = sub_0813F7B4(p);

    q += 0x428;
    if (*(u16 *)q >= r) {
        return 1;
    }
    return 0;
}
