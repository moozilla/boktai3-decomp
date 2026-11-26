#include "global.h"
u32 Save_AlignSizeToEightBytes(u32 x)
{
    u32 r = x & ~7;
    if ((x & 7) != 0)
        r += 8;
    return r;
}
