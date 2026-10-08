#include "global.h"

void sub_0811CD24(u8 *, u32);

void sub_0810D14C(u8 *p)
{
    u32 flag = 1;
    u8 *q = p + 0x28;
    u8 *r;
    s32 i;

    for (i = 6; i >= 0; i--) {
        *(u32 *)q |= flag;
        q += 0x60;
    }
    r = p + 0x940;
    for (i = 1; i >= 0; i--) {
        sub_0811CD24(r, 1);
        r += 0x850;
    }
}
