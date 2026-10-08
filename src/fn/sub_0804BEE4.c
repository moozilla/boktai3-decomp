#include "global.h"
u32 sub_0804BEE4(s32 a, s32 b)
{
    if (a < 0) a = -a;
    if (a > 0x20) return 0;
    if (b < 0) b = -b;
    if (b > 0x20) return 0;
    return 1;
}
