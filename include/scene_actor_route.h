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
u32 sub_0821E618(void *);
u32 sub_0821E8C8(struct SceneActorRouteCursor *, const struct ContactPosition *, u32, u32);
#endif
