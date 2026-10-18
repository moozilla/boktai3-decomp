#include "global.h"
void Position_GetHeightAdjustedCoordinates(u32, u32, s16 *);
void sub_0811B164(u32, s32, s32, s32);
void Collider_SetFixedPositionWithYOffset(u32 s, u32 a, u32 b)
{
    s16 buf[3];
    s16 *v = buf;
    Position_GetHeightAdjustedCoordinates(a, b, v);
    sub_0811B164(s, buf[0], v[1], v[2]);
}
