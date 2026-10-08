#include "global.h"
u8 *sub_08019B30(u8 *p)
{
    u8 *e = p + 0x18;
    s32 i = 0;
    u8 *r;
    do {
        if (e[4] != 0) {
            i++;
            e += 0xb0;
            continue;
        }
        r = e;
        goto end;
    } while (i <= 0xf);
    r = 0;
end:
    return r;
}
