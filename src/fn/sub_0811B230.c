#include "global.h"
void Position_GetHeightAdjustedCoordinates(u32, u32, s16 *);
void SpatialIndex_SetPosition(u32, s32, s32, s32);
void Collider_SetFixedPositionWithYOffset(u32 s, u32 a, u32 b)
{
    s16 buf[3];
    s16 *v = buf;
    Position_GetHeightAdjustedCoordinates(a, b, v);
    SpatialIndex_SetPosition(s, buf[0], v[1], v[2]);
}
