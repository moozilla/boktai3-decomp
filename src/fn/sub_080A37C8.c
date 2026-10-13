#include "global.h"

void sub_080A37C8(u8 *p)
{
    if (p[0x2b1])
        p[0x2b1] = 0;
    if (!(*(u8(**)(u8 *))(p + 0x270))(p))
        (*(u32 *)(p + 0x2ac))++;
}
