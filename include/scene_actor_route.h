#ifndef GUARD_SCENE_ACTOR_ROUTE_H
#define GUARD_SCENE_ACTOR_ROUTE_H
#include "scene_actor_registry.h"
/* Prefix of the actor's embedded cursor at448; the reset-phase route view
 * continues beyond these twelve bytes. Point positions store X/Z, not XYZ. */
struct SceneActorRouteDefinition {
    u8 pointCount, unknown1;
    u16 pointOffset;
};
struct SceneActorRoutePoint {
    s16 x, z;
    u8 unknown4[2], attributes, unknown7;
};
struct SceneActorRouteCursor {
    u8 unknown0[2], pointIndex, unknown3;
    struct SceneActorRouteDefinition *definition;
    struct SceneActorRoutePoint *point;
};
u32 Route_AdvancePoint(void *);
u32 sub_0821E8C8(struct SceneActorRouteCursor *, const struct ContactPosition *, u32, u32);
/* Tentative destination object referenced by the actor's word at 0xDC. */
struct SceneActorRouteDestination {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
/* Route context referenced by actor offset 0x3D0. Consumers 080C4CFC and
 * 080C4DBC copy destination words after terrain/path checks. The latter sets
 * bit 19 of flags628; the flag's broader meaning remains unknown. */
struct SceneActorRouteContext {
    u8 unknown0[0x628];
    u32 flags628;
    u8 unknown62C[0x68];
    struct ContactPosition destination;
};
#endif
