#include "global.h"
s32 sub_08197DD0(u32 a)
{
    s32 r;
    switch (a) {
    case 3:
        r = 1;
        break;
    case 0:
    case 4:
    case 5:
        r = 2;
        break;
    case 1:
    case 2:
    default:
        r = 0;
        break;
    }
    return r;
}
