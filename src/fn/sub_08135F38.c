#include "global.h"
#include "scene_actor_reset.h"

static inline short ClearReaction(u16 *flags, u32 mask)
{
    return mask & *flags;
}
void SceneActor_ClearReactionFlags(struct ChaseActor *actor)
{
    if (actor->base.hitReactionFlags & 0xC4000) {
        actor->base.reactionFlags = ClearReaction(&actor->base.reactionFlags, ~0x9C9);
        actor->base.hitLockTimer = 0;
    }
}
