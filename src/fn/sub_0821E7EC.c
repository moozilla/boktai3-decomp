#include "global.h"
void Path_GetDefinition(u32);
s32 Path_GetPointData(void);
void Path_GetPointPosition(u16 *, s32, u32);
s32 Path_GetPointPositionById(u16 *a, u32 b, u32 c)
{
    s32 r;
    s32 x;
    Path_GetDefinition(b);
    x = Path_GetPointData();
    if (x != 0) {
        Path_GetPointPosition(a, x, c);
        r = 0;
    } else {
        a[0] = x;
        a[1] = x;
        a[2] = x;
        r = -1;
    }
    return r;
}
