#include "global.h"

s32 sub_0814DDB4(u32 x)
{
    s32 r;
    switch (x) {
    case 2:
        r = 2;
        break;
    case 5:
        r = 5;
        break;
    case 3:
        r = 3;
        break;
    case 4:
        r = 4;
        break;
    case 1:
    case 6:
        r = 1;
        break;
    case 8:
        r = 6;
        break;
    case 0:
    case 7:
    default:
        r = 0;
        break;
    }
    return r;
}
