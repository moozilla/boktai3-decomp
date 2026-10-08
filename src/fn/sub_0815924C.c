#include "global.h"

s32 sub_0815924C(u8 *p)
{
    u8 v = p[0x457];

    if (v == 0xC || v == 0x26) {
        return 1;
    }
    return 0;
}
