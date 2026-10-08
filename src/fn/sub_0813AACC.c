#include "global.h"
u8 *sub_0813AACC(u8 *p)
{
    s32 i = 0;
    u32 one = 1;
    u8 *res;
    u32 *r = (u32 *)(p + 0x53c);
    u8 *q = p + 0x51c;
    do {
        if (*r == 1) {
            r = (u32 *)((u8 *)r + 0x24);
            q += 0x24;
            i++;
            continue;
        }
        q[0x17] = i;
        *r = one;
        res = q;
        goto end;
    } while (i <= 0x1f);
    res = 0;
end:
    return res;
}
