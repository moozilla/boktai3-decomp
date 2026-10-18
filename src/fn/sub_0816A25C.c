// Tentative StatusPanel family and semantic names; based on static consumers.
#define StatusPanel_GetDirectionAxis sub_0816A25C
#include "global.h"
s32 sub_0816A25C(s32 direction)
{
    s32 result;
    switch (direction >> 2) {
    case 2:
        result = 2;
        break;
    case 1:
    case 3:
        result = 1;
        break;
    case 0:
    default:
        result = 0;
        break;
    }
    return result;
}
