#ifndef GUARD_SCENE_ACTOR_COLLISION_H
#define GUARD_SCENE_ACTOR_COLLISION_H
#include "scene_actor_registry.h"
/* Views into actor offsets9E andB4. Property names remain offset-based where
 * the collision-engine consumer has not established their full meaning. */
struct SceneActorCollisionDefaults {
    u8 cost, unknown1[3];
    u16 propertyA2, reactionFlags;
};
struct SceneActorCollisionParameters {
    u16 cost[4], flags40[4], field44[4];
    u32 property38[4];
};
void sub_0821FF7C(void *, u16, u32, u32);
static inline u16 SceneActorCollision_ReadCost(struct SceneActorRegistryActor *actor)
{
    const u8 *field = (u8 *)actor + 0x9e;
    return *field;
}
static inline u32 SceneActorCollision_ReadProperty(struct SceneActorRegistryActor *actor)
{
    const u16 *field = (void *)((u8 *)actor + 0xa2);
    return *field;
}
static inline u32 SceneActorCollision_ReadReaction(struct SceneActorRegistryActor *actor)
{
    const u16 *field = (void *)((u8 *)actor + 0xa4);
    return *field;
}
#endif
