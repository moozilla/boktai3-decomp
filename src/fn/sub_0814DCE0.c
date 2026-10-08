#include "global.h"

s32 sub_0814DCE0(u32 x)
{
    s32 r;
    switch (x) {
    case 2:
        r = 3;
        break;
    case 3:
        r = 4;
        break;
    case 4:
        r = 5;
        break;
    case 5:
        r = 6;
        break;
    case 1:
    case 6:
        r = 7;
        break;
    case 0:
    case 7:
        r = 8;
        break;
    case 8:
        r = 9;
        break;
    default:
        r = 0;
        break;
    }
    return r;
}
