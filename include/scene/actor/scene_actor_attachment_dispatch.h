#ifndef GUARD_SCENE_ACTOR_ATTACHMENT_DISPATCH_H
#define GUARD_SCENE_ACTOR_ATTACHMENT_DISPATCH_H
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_attachment.h"
#include "scene/actor/actor_attachment_manager.h"

static inline u32 SceneActorAttachment_MotionBits(struct ChaseActor *actor, u32 mask)
{
    u16 *p = &actor->base.motionFlags;
    return *p & mask;
}

static inline u32 SceneActorAttachment_StatusBits(struct ChaseActor *actor, u32 mask)
{
    u32 *p = &actor->base.statusFlags;
    return *p & mask;
}

static inline void SceneActorAttachment_ReleaseSpriteSlots(struct ChaseActor *actor, void **slot)
{
    *slot = ActorAttachment_ReleaseResource(*slot, 0, (void *)actor->base.body.id);
    *slot = ActorAttachment_ReleaseResource(*slot, 1, (void *)actor->base.body.id);
    *slot = ActorAttachment_ReleaseResource(*slot, 2, (void *)actor->base.body.id);
}
#endif
