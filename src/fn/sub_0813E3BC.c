#include "global.h"

s32 sub_0813E3BC(s32 unused0, s32 unused1, u8 *p)
{
    u32 m = 0x80;
    m <<= 4;
    if (*(u32 *)(p + 0x38) & m) {
        return 10;
    }
    return 0;
}
