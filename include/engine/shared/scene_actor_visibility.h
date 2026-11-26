#ifndef GUARD_SCENE_ACTOR_VISIBILITY_H
#define GUARD_SCENE_ACTOR_VISIBILITY_H

#include "engine/shared/scene_actor_registry.h"
#include "engine/shared/scene_actor_state.h"

static inline u8 SceneActorVisibility_IsHidden(struct ChaseActor *actor)
{
    u32 mask = 1;
    if (!actor->base.drawableKind) {
        if (((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->flags & mask)
            goto hidden;
        goto visible;
    }
    if (!(((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->flags & mask))
        goto visible;
hidden:
    return 1;
visible:
    return 0;
}

#endif
