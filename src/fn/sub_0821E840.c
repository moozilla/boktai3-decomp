#include "global.h"
void *Path_GetDefinition(s32);
void *Path_GetPointData(void *);
s32 Path_GetPointNodeId(void *, s32);

s32 sub_0821E840(s32 path, s32 point)
{
    void *data = Path_GetPointData(Path_GetDefinition(path));
    s32 result;
    if (data != NULL)
        result = Path_GetPointNodeId(data, point);
    else
        result = 0;
    return result;
}
