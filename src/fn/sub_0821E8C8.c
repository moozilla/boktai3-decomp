#include "global.h"
#include "scene_actor_route.h"

union RouteCoordinateWord {
    struct {
        s16 value, padding;
    } coordinate;
    u32 word;
};
struct RouteBoundsPoint {
    union RouteCoordinateWord x, z;
};
u32 SceneActorRoute_IsOutsidePointBounds(struct SceneActorRouteCursor *cursor,
                                         const struct ContactPosition *position, u32 xBound,
                                         u32 zBound)
{
    struct RouteBoundsPoint point;
    s32 dx, dz;
    u32 magnitude;
    point.x.coordinate.value = cursor->point->x;
    point.z.coordinate.value = cursor->point->z;
    {
        const s16 pointZ[1] = {point.z.coordinate.value};
        dx = position->x - point.x.coordinate.value;
        if (dx < 0)
            dx = -dx;
        magnitude = dx;
        if (magnitude > xBound)
            return 1;
        dz = position->z - (s16)pointZ[0];
        magnitude = dz;
        if (dz < 0)
            magnitude = -dz;
        if (magnitude > zBound)
            return 1;
        return 0;
    }
}