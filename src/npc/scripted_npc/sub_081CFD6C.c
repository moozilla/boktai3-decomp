#include "global.h"
s32 Math_Atan2(s32, s32);
s32 sub_081CFD6C(s32 *a, s32 *b)
{
    return Math_Atan2(b[0] - a[0], b[1] - a[1]);
}
