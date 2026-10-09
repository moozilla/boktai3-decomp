#include "global.h"

s32 sub_0817AA5C(u8 *p)
{
    u8 *q = p + 0x476;
    s32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}
