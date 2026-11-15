#include "scene_actor_control_helpers.h"
#include "scene_actor_control_phase.h"
#include "player.h"
#include "scene_actor_reaction.h"
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline void AndWordFlags(u32 *flags, u32 mask)
{
    *flags &= mask;
}
static inline void OrWordFlags(u32 mask, u32 *flags)
{
    *flags |= mask;
}
static inline void OrTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = bits;
    mask |= tracking->flags;
    tracking->flags = mask;
}
static inline u8 TrackingHas(struct SceneActorChaseTracking *tracking, u32 mask)
{
    if (tracking->flags & mask)
        return 1;
    return 0;
}
static inline void SelectState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                               u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
}
void SceneActorControl_ConvergeSavedMotion(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        {
            u16 mask = 0x20;
            u16 *flags = &actor->flags2D4;
            *flags = mask | *flags;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            struct ContactCollider *collider = &actor->colliderB;
            collider->flags |= 4;
        }
    }
    OrWordFlags(0x100000, &actor->flags2D0);
    {
        s32 current = actor->reactionStart.x;
        s16 *field = &actor->reactionEnd.x;
        s32 difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        s32 current = actor->reactionStart.z;
        s16 *field = &actor->reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        u16 *flags;
        struct SceneActorChaseTracking *tracking;
        u32 tracked;
        actor->field2B0 = 1;
        flags = &actor->flags2D4;
        {
            u32 mask = ~0x20;
            mask &= *flags;
            tracked = 0;
            *flags = mask;
        }
        AndWordFlags(&actor->flags2CC, ~0x80);
        state->phase = tracked;
        tracking = &actor->tracking;
        tracked = TrackingHas(tracking, 8);
        if (tracked) {
            SceneActorReaction_EnableMotionFlags(0x100, flags);
            SceneActorControl_StartPositionCurve(actor, &gPlayers[0]->position);
            SelectState(actor, 0, 2, 0);
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            SceneActorControl_ResetCollisionPhase(actor);
        } else {
            OrTrackingFlags(tracking, 8);
            SceneActorReaction_EnableMotionFlags(0x100, flags);
            SelectState(actor, 0, 0, 3);
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            SceneActorControl_ResetCollisionPhase(actor);
        }
    }
}
