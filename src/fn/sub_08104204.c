#include "global.h"

s32 Pool184_TestStatusFlags(u8 *p, u32 mask)
{
    if (*(u16 *)(p + 0x156) & mask)
        return 1;
    return 0;
}
