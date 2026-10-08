#include "global.h"
u8 *sub_08126AA4(u8 *p)
{
    s32 i = 0;
    u32 one = 1;
    u8 *res;
    u8 *r = p + 0x76;
    u8 *q = p + 0x38;
    do {
        if (r[1] == 1) {
            r += 0x40;
            q += 0x40;
            i++;
            continue;
        }
        r[0] = i;
        r[1] = one;
        res = q;
        goto end;
    } while (i <= 0x7f);
    res = 0;
end:
    return res;
}
