#ifndef GUARD_SCENE_ACTOR_VISIBILITY_H
#define GUARD_SCENE_ACTOR_VISIBILITY_H

#include "scene_actor_registry.h"
#include "scene_actor_state.h"

static inline u8 SceneActorVisibility_IsHidden(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 1;
    if (!actor->drawableKind) {
        if (((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags & mask)
            goto hidden;
        goto visible;
    }
    if (!(((struct SceneActorRegistryTileDrawable *)actor->drawable)->flags & mask))
        goto visible;
hidden:
    return 1;
visible:
    return 0;
}

#endif
