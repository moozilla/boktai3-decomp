#include "global.h"
u32 CollectionMenu_IsValidGridPosition(u32 a, s32 b, s32 c)
{
    if (a <= 4 && b >= 0 && b <= 1) {
        if ((s32)(c * 10 + b * 5 + a) <= 26)
            return 1;
    }
    return 0;
}
