#include "global.h"

extern u8 *gUnk_020001AC;

u8 *sub_08101A74(void)
{
    u8 *g = gUnk_020001AC;
    s32 i = 0;
    u32 v = *(u32 *)(g + 0x1370);
    u8 *a = g + 0x1c;
    u8 *b = g + 0xf8;
    u8 *r;
    do {
        if (v == *(u32 *)b) {
            r = a;
            goto end;
        }
        a += 0x19c;
        b += 0x19c;
        i++;
    } while (i <= 11);
    r = 0;
end:
    return r;
}
