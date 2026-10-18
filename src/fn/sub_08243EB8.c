#include "global.h"
extern u32 gUnk_03006A38;
extern s32 gUnk_03006A48;
s32 Solar_GetMeasurement(void)
{
    s32 r;
    if (gUnk_03006A38 != 0)
        r = gUnk_03006A48;
    else
        r = -1;
    return r;
}
