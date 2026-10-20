#include "global.h"
#include "scene_actor_reset.h"

static inline short ClearReaction(u16 *flags, u32 mask)
{
    return mask & *flags;
}
void SceneActor_ClearReactionFlags(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2DC & 0xC4000) {
        actor->flags2D6 = ClearReaction(&actor->flags2D6, ~0x9C9);
        actor->field2B8 = 0;
    }
}
