#include "global.h"
u8 *sub_081B6810(u8 *p, s32 *out)
{
    s32 i = 0;
    u32 one = 1;
    u32 f = *(u32 *)(p + 0x18);
    u8 *q = p + 0x24;
    u8 *r;
    do {
        if (!((one << i) & f)) {
            *out = i;
            r = q;
            goto end;
        }
        q += 0x234;
        i++;
    } while (i <= 7);
    r = 0;
end:
    return r;
}
