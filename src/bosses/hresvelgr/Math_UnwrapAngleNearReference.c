#include "global.h"
/* Math_UnwrapAngleNearReference: Returns an equivalent byte angle within half a turn of the reference angle */

s32 Math_UnwrapAngleNearReference(s32 angle, s32 ref)
{
    s16 d = (angle - ref) & 0xff;
    if (d > 128)
        d -= 256;
    return ref + d;
}
