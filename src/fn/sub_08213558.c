#include "global.h"
u32 sub_08213558(u32 a, s32 b, s32 c)
{
    if (a <= 4 && b >= 0 && b <= 1) {
        if ((s32)(c * 10 + b * 5 + a) <= 9)
            return 1;
    }
    return 0;
}
