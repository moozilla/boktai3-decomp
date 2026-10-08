#include "global.h"

void sub_0811CD24(u8 *, u32);

void sub_0810FD90(u8 *p)
{
    u32 flag = 1;
    u8 *q = p + 0x2C;
    s32 i;

    for (i = 0x11; i >= 0; i--) {
        *(u32 *)q |= flag;
        q += 0x60;
    }
    sub_0811CD24(p + 0xD8C, 1);
}
