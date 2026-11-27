#include "global.h"
s32 Solar_GetLevelForIntensity(s32 v)
{
    if (v == 0)
        return 0;
    if (v <= 5)
        return 1;
    if (v <= 12)
        return 2;
    if (v <= 22)
        return 3;
    if (v <= 34)
        return 4;
    if (v <= 49)
        return 5;
    if (v <= 66)
        return 6;
    if (v <= 86)
        return 7;
    if (v <= 109)
        return 8;
    if (v <= 139)
        return 9;
    return 10;
}
