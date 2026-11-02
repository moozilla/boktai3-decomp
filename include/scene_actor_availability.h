#ifndef GUARD_SCENE_ACTOR_AVAILABILITY_H
#define GUARD_SCENE_ACTOR_AVAILABILITY_H
#include "scene_actor_registry.h"
static inline u32 SceneActor_MaskedHalfword(u32 mask, const u16 *flags)
{
    return *flags & mask;
}
static inline u32 SceneActor_MaskedWord(u32 mask, const u32 *flags)
{
    return *flags & mask;
}
static inline u8 SceneActor_ActionsBlocked(const struct SceneActorRegistryActor *actor)
{
    if (!actor)
        return 0;
    if (SceneActor_MaskedHalfword(0x40, &actor->flags2D4) ||
        SceneActor_MaskedWord(0x20000, &actor->flags2D0) || actor->health <= 0)
        return 1;
    return 0;
}
static inline u8 SceneActor_TestPendingFlag(struct SceneActorRegistryActor *actor, u32 mask,
                                            u16 **saved)
{
    u32 bits = actor->flags2D6 & mask;
    *saved = &actor->flags2D6;
    if (bits)
        return 1;
    return 0;
}
#endif
