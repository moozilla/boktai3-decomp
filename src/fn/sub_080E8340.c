#include "global.h"
#include "scene_actor_reaction.h"
void sub_080E8158(struct SceneActorRegistryResetView *);
static inline u32 actorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 reactionStarted(struct SceneActorRegistryResetView *actor,
                                 struct SceneActorReactionMotion **out)
{
    struct SceneActorReactionMotion *motion = (void *)&actor->tracking.target;
    u32 mask = 8;
    u32 flags = motion->flags & mask;
    *out = motion;
    if (flags)
        return 1;
    return 0;
}
static inline s32 motionSquared(struct ContactPosition *motion)
{
    return motion->x * motion->x + motion->y * motion->y + motion->z * motion->z;
}
static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}
static inline void prepareControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind,
                                  s32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    clearFlag(&actor->flags114);
    actor->word104 = zero;
}
void SceneActor_PrepareStationaryReaction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionMotion *motion;
    s32 offset;
    u8 started;
    if (actorFlags(actor, 0x80)) {
        actor->group.field418 = actor->group.field416;
        return;
    }
    started = reactionStarted(actor, &motion);
    if (started)
        return;
    if (motionSquared(&actor->secondaryMotion) || (offset = *(s16 *)&actor->field2EA)) {
        sub_080E8158(actor);
    } else {
        prepareControl(actor, SceneActorControl_OnIdle, 0x10, offset);
        actor->state2A8 = 0;
        actor->phase2A9 = 1;
        actor->behavior2AA = 0;
        actor->counter2AC = offset;
        actor->field2B1 = 1;
    }
    {
        u32 mask = 8;
        mask |= motion->flags;
        motion->flags = mask;
    }
}
