#include "global.h"
extern u8 *gUnk_02000710;
u8 *sub_0806F9AC(void)
{
    s32 i = 0;
    u8 *q = gUnk_02000710 + 0x670;
    u8 *r;
    do {
        if (*(u16 *)(q + 2) == 0) {
            r = q;
            goto end;
        }
        q += 6;
        i++;
    } while (i <= 0x13);
    r = 0;
end:
    return r;
}
