#include "global.h"
u8 *sub_081F21C0(u8 *p, s32 *out)
{
    s32 i = 0;
    u32 one = 1;
    u32 f = *(u32 *)(p + 0x18);
    u8 *q = p + 0x20;
    u8 *r;
    do {
        if (!((one << i) & f)) {
            *out = i;
            r = q;
            goto end;
        }
        q += 0x258;
        i++;
    } while (i <= 3);
    r = 0;
end:
    return r;
}
