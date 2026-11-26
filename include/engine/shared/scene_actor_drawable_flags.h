#ifndef GUARD_SCENE_ACTOR_DRAWABLE_FLAGS_H
#define GUARD_SCENE_ACTOR_DRAWABLE_FLAGS_H

#include "engine/shared/scene_actor_chase_shared.h"
#include "engine/shared/scene_actor_registry.h"

static inline void ActorStatus_SetDrawableFlags(struct ChaseActor *actor, u32 bits)
{
    if (actor->base.drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->flags |= bits;
    else {
        struct SceneInputWidget *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        tile->flags |= bits;
    }
}

static inline void ActorStatus_ClearTileFlags(struct SceneInputWidget *tile, u32 bits)
{
    u32 keep = ~bits;
    tile->flags &= keep;
}

static inline void ActorStatus_ClearDrawableFlags(struct ChaseActor *actor, u32 bits)
{
    if (actor->base.drawableKind == 0)
        SceneActorChase_ClearWordFlags(
            &((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->flags, bits);
    else {
        struct SceneInputWidget *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        ActorStatus_ClearTileFlags(tile, bits);
    }
}

static inline void ActorStatus_SetBodyBit(struct ChaseActor *actor, u32 on)
{
    if (on)
        ActorStatus_SetDrawableFlags(actor, 1);
    else
        ActorStatus_ClearDrawableFlags(actor, 1);
}

#endif
