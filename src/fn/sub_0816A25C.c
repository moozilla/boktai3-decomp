// Tentative StatusPanel family and semantic names; based on static consumers.
#include "global.h"
s32 StatusPanel_GetDirectionAxis(s32 direction)
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
