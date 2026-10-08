#include "global.h"

void sub_0810C14C(u8 *p)
{
    u32 flag = 1;
    u8 *q = p + 0x28;
    s32 i;

    for (i = 6; i >= 0; i--) {
        *(u32 *)q |= flag;
        q += 0x60;
    }
}
