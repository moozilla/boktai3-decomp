#ifndef GUARD_SCENE_ACTOR_ATTACHMENT_DISPATCH_H
#define GUARD_SCENE_ACTOR_ATTACHMENT_DISPATCH_H
#include "scene_actor_reset.h"
#include "scene_actor_attachment.h"
#include "actor_attachment_manager.h"

static inline u32 SceneActorAttachment_MotionBits(struct SceneActorRegistryResetView *actor,
                                                  u32 mask)
{
    u16 *p = &actor->flags2D4;
    return *p & mask;
}

static inline u32 SceneActorAttachment_StatusBits(struct SceneActorRegistryResetView *actor,
                                                  u32 mask)
{
    u32 *p = &actor->flags2D0;
    return *p & mask;
}

static inline void
SceneActorAttachment_ReleaseSpriteSlots(struct SceneActorRegistryResetView *actor, void **slot)
{
    *slot = ActorAttachment_ReleaseResource(*slot, 0, (void *)actor->body.id);
    *slot = ActorAttachment_ReleaseResource(*slot, 1, (void *)actor->body.id);
    *slot = ActorAttachment_ReleaseResource(*slot, 2, (void *)actor->body.id);
}
#endif
