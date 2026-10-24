#include "global.h"
#include "scene_actor_route.h"

// MATCHING_CONCESSION: register-pinning
u32 SceneActorRoute_IsOutsidePointBounds(struct SceneActorRouteCursor *cursor,
                                         const struct ContactPosition *position, u32 xBound,
                                         u32 zBound)
{
    struct RouteBoundsCoordinate {
        s16 value, padding;
    };
    register struct {
        struct RouteBoundsCoordinate x, z;
    } point asm("r4");
    struct RouteBoundsCoordinate savedZ;
    s32 distance;
    s32 positionX;
    register s32 pointX asm("r0");

    point.x.value = cursor->point->x;
    point.z.value = cursor->point->z;
    savedZ = point.z;
    positionX = position->x;
    pointX = point.x.value;
    distance = positionX - pointX;
    if (distance < 0)
        distance = -distance;
    if ((u32)distance > xBound)
        return 1;
    distance = position->z - savedZ.value;
    if (distance < 0)
        distance = -distance;
    if ((u32)distance > zBound)
        return 1;
    return 0;
}
