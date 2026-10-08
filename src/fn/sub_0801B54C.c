#include "global.h"
u32 sub_0801B54C(u8 *p)
{
    u32 r;
    u8 *q = p + 0x38;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}
