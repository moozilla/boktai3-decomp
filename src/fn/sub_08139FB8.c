#include "global.h"
void sub_08139FB8(u8 *p)
{
    u8 *q = p + 0xcf;
    if (*q != 0) {
        u8 *r;
        u32 z;
        *(u32 *)(p + 0x60) |= 1;
        r = p + 0xce;
        z = 0;
        *r = z;
        *(u32 *)p = z;
        *q = z;
    }
}
