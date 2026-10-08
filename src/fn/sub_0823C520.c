#include "global.h"

s32 sub_0823C520(s32 x)
{
    if (x == 12)
        return 0x40;
    if (x <= 11)
        return ((12 - x) << 3) + 0x40;
    return 0x40 - ((x - 12) << 2);
}
