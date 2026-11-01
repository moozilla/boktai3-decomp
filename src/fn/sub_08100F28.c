#include "named_object.h"

u32 NamedObject_IsWithinRoutePointBounds(struct SceneActorRouteCursor *cursor,
                                         const struct ContactPosition *position, u32 xBound,
                                         u32 zBound)
{
    struct SceneActorRoutePoint *point = cursor->point;
    s32 px = (u16)point->x;
    s32 bx = position->x;
    s32 pz = (u16)point->z;
    s32 bz = position->z;
    s32 dz = pz - bz;
    s32 dx = px - bx;
    if (dx < 0)
        dx = -dx;
    if (dx <= xBound) {
        if (dz < 0)
            dz = -dz;
        if (dz <= zBound)
            return 1;
    }
    return 0;
}
