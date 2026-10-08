#include "global.h"

void sub_0813DD84(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x348);

    if (*(u16 *)(q + 2) & 0xF3) {
        p[0x4F4]++;
    }
}
