#include "global.h"
void sub_08138AF8(u8 *p)
{
    u8 *q = p + 0xd4;
    if (*q != 0) {
        u8 *r = p + 0xd5;
        u32 z = 0;
        *r = z;
        *q = z;
        *(u32 *)p = z;
    }
}
