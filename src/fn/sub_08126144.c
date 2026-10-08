#include "global.h"
u8 *sub_08126144(u8 *s)
{
    s32 i = 0;
    u8 *a = s + 0x88;
    u8 *b = s + 0x1c;
    u8 *r;
    do {
        if (*(s8 *)a >= 0) {
            a += 0x70;
            b += 0x70;
            i++;
        } else {
            *a = i;
            r = b;
            goto end;
        }
    } while (i <= 0x2f);
    r = 0;
end:
    return r;
}
