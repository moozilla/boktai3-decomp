#ifndef GUARD_SCENE_ACTOR_COLLISION_H
#define GUARD_SCENE_ACTOR_COLLISION_H
#include "scene_actor_registry.h"
/* Views into actor offsets9E andB4. Property names remain offset-based where
 * the collision-engine consumer has not established their full meaning. */
struct SceneActorCollisionDefaults {
    u8 field3C, unknown1[3];
    u16 reactionFlags, field34;
};
struct SceneActorCollisionParameters {
    u16 field3C[4], field40[4], field44[4];
    u32 reactionFlags[4];
};
void sub_0821FF7C(void *, u16, u32, u32);
static inline u16 SceneActorCollision_ReadBodyField3C(struct SceneActorRegistryActor *actor)
{
    const u8 *field = (u8 *)actor + 0x9e;
    return *field;
}
static inline u32 SceneActorCollision_ReadBodyReactionFlags(struct SceneActorRegistryActor *actor)
{
    const u16 *field = (void *)((u8 *)actor + 0xa2);
    return *field;
}
static inline u32 SceneActorCollision_ReadBodyField34(struct SceneActorRegistryActor *actor)
{
    const u16 *field = (void *)((u8 *)actor + 0xa4);
    return *field;
}
void sub_0821FF58(void *, u16, u16, u32, u32, u16);
/* GNU trailing-storage views avoid imposing a four-entry index bound. The
 * selected entries must lie in initialized storage in the actual actor object. */
static inline u16 SceneActorCollision_ReadAttackField3C(struct SceneActorRegistryActor *actor,
                                                        u32 index)
{
    const struct {
        u8 unknown0[0xb4];
        u16 entries[0];
    } *view = (const void *)actor;
    return view->entries[index];
}
static inline u16 SceneActorCollision_ReadAttackField40(struct SceneActorRegistryActor *actor,
                                                        u32 index)
{
    const struct {
        u8 unknown0[0xbc];
        u16 entries[0];
    } *view = (const void *)actor;
    return view->entries[index];
}
static inline u32 SceneActorCollision_ReadAttackReactionFlags(struct SceneActorRegistryActor *actor,
                                                              u32 index)
{
    const struct {
        u8 unknown0[0xcc];
        u32 entries[0];
    } *view = (const void *)actor;
    return view->entries[index];
}
static inline u16 SceneActorCollision_ReadAttackField44(struct SceneActorRegistryActor *actor,
                                                        u32 index)
{
    const struct {
        u8 unknown0[0xc4];
        u16 entries[0];
    } *view = (const void *)actor;
    return view->entries[index];
}
#endif
