#include "global.h"
#include "scene_actor_route.h"

#ifdef NONMATCHING
// fuzzy: 95.5%
// foundry draft (worker best 95.5%); what differs is mostly register allocation and instruction order
u32 SceneActorRoute_IsOutsidePointBounds(struct SceneActorRouteCursor *cursor,
                                         const struct ContactPosition *position, u32 xBound,
                                         u32 zBound)
{
    struct ContactPackedVector point;
    s32 distance;
    point.x = cursor->point->x;
    point.z = cursor->point->z;
    distance = position->x - point.x;
    if (distance < 0)
        distance = -distance;
    if ((u32)distance > xBound)
        return 1;
    distance = position->z - point.z;
    if (distance < 0)
        distance = -distance;
    if ((u32)distance > zBound)
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorRoute_IsOutsidePointBounds);
#endif
