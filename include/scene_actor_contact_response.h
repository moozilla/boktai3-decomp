#ifndef GUARD_SCENE_ACTOR_CONTACT_RESPONSE_H
#define GUARD_SCENE_ACTOR_CONTACT_RESPONSE_H

#include "scene_actor_chase_reaction.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_reaction.h"

struct SceneActorContactTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};

struct SceneActorContactTracking {
    struct SceneActorChaseTracking tracking;
    u32 active;
    u32 *actionFlags;
};

static inline u32 SceneActorContact_ReadActorFlags(struct SceneActorRegistryResetView *actor,
                                                   u32 mask, u32 **field)
{
    u32 *flags = &actor->flags2CC;
    u32 value = *flags & mask;
    *field = flags;
    return value;
}
static inline u32 SceneActorContact_MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 SceneActorContact_TestTracking(struct SceneActorContactTracking *motion, u32 mask)
{
    if (motion->tracking.flags & mask)
        return 1;
    return 0;
}
static inline void SceneActorContact_SetTrackingFlags(struct SceneActorContactTracking *motion,
                                                      u8 bits)
{
    motion->tracking.flags = bits | motion->tracking.flags;
}
static inline u16 SceneActorContact_SavedTargetCell(struct SceneActorRegistryResetView *actor)
{
    return *(u16 *)actor->unknown2F8;
}
static inline void SceneActorContact_SaveTargetCell(struct SceneActorRegistryResetView *actor,
                                                    u16 cell)
{
    *(u16 *)actor->unknown2F8 = cell;
}

static inline u8 SceneActorContact_TestTrackingOwned(struct SceneActorContactTracking *current,
                                                     u32 mask,
                                                     struct SceneActorContactTracking **motion)
{
    u32 value = current->tracking.flags & mask;
    *motion = current;
    if (value)
        return 1;
    return 0;
}
static inline u32 SceneActorContact_QueryMotionField(struct SceneActorRegistryResetView *actor,
                                                     u16 mask, void **owner)
{
    u16 *field = &actor->flags2D4;
    u32 value;
    *owner = field;
    value = *field & mask;
    return value;
}

static inline u16 *SceneActorContact_MotionField(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
}
static inline struct SceneActorChaseFlags16 *SceneActorContact_GateFromWork(void *const *work)
{
    return &((struct SceneActorRegistryResetView *)*work)->group.common.gate.flags;
}
static inline struct SceneActorRegistryResetView *SceneActorContact_ActorFromWork(void *const *work)
{
    return *work;
}
#define SCENE_ACTOR_CONTACT_READ_MOTION_FIELD(out_, field_)                                        \
    do {                                                                                           \
        (out_) = *(field_);                                                                        \
    } while (0)
#define SCENE_ACTOR_CONTACT_FIND_CONTACT(out_, actor_)                                             \
    do {                                                                                           \
        (out_) = SceneActorContacts_SelectForActor((void *)(actor_), 6);                           \
    } while (0)

#endif
