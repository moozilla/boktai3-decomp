#ifndef GUARD_SCENE_ACTOR_AVAILABILITY_H
#define GUARD_SCENE_ACTOR_AVAILABILITY_H
#include "scene/actor/scene_actor_registry.h"
#include "engine/word_flags.h"
static inline u32 SceneActor_MaskedHalfword(u32 mask, const u16 *flags)
{
    return *flags & mask;
}
static inline u8 SceneActor_ActionsBlocked(const struct ChaseActor *actor)
{
    if (!actor)
        return 0;
    if (SceneActor_MaskedHalfword(0x40, &actor->base.motionFlags) ||
        Flags_MaskedWord(ROOM_ACTOR_STATUS_INACTIVE, &actor->base.statusFlags) ||
        actor->base.health <= 0)
        return 1;
    return 0;
}
static inline u8 SceneActor_TestPendingFlag(struct ChaseActor *actor, u32 mask, u16 **saved)
{
    u32 bits = actor->base.reactionFlags & mask;
    *saved = &actor->base.reactionFlags;
    if (bits)
        return 1;
    return 0;
}
#endif
