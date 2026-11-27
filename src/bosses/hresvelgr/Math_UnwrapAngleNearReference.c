#include "global.h"
/* Math_UnwrapAngleNearReference: Returns an equivalent byte angle within half a turn of the reference angle */

s32 Math_UnwrapAngleNearReference(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    s32 x = d;
    if (x > 128)
        d = x - 256;
    return ref + d;
}
