#include "global.h"

s32 sub_08003860(u32 *p, s32 x)
{
    switch (x) {
    case 1:
        if (*p > 2)
            return -1;
        *p = 1;
        break;
    case 2:
        if (*p > 2)
            return -1;
        *p = 2;
        break;
    case 3:
        *p = 3;
        break;
    case 4:
    case 5:
        *p = 4;
        break;
    }
    return 0;
}
