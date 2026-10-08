#include "global.h"
u8 *sub_0811B828(u8 *s)
{
    s32 i = 0;
    u8 *p = s + 0x18;
    u8 *r;
    do {
        if (*(u32 *)(p + 0x44) == 1) {
            p += 0x48;
            i++;
        } else {
            r = p;
            goto end;
        }
    } while (i <= 11);
    r = 0;
end:
    return r;
}
