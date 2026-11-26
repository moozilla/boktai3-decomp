#include "scene/actor/scene_actor_control_helpers.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "player/player.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/actor/scene_actor_chase_shared.h"
#include "engine/word_flags.h"
#include "scene/actor/scene_actor_displacement.h"
void SceneActorControl_ResetCollisionPhase(struct ChaseActor *);
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
static inline void OrTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = bits;
    mask |= tracking->flags;
    tracking->flags = mask;
}
static inline void SelectState(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
}
void SceneActorControl_ConvergeSavedMotion(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        {
            u16 mask = 0x20;
            u16 *flags = &actor->base.motionFlags;
            *flags = mask | *flags;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            struct ContactCollider *collider = &actor->base.colliderB;
            collider->flags |= 4;
        }
    }
    WordFlags_Set(0x100000, &actor->base.statusFlags);
    {
        s32 current = actor->base.reactionStart.x;
        s16 *field = &actor->base.reactionEnd.x;
        s32 difference = current - *field;
        priorX = field;
        x = SceneActorDisplacement_Quarter(difference);
    }
    {
        s32 current = actor->base.reactionStart.z;
        s16 *field = &actor->base.reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = SceneActorDisplacement_Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->base.body.motion.x += x;
    actor->base.body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        u16 *flags;
        struct SceneActorChaseTracking *tracking;
        u32 tracked;
        actor->base.stepDone = 1;
        flags = &actor->base.motionFlags;
        {
            u32 mask = ~0x20;
            mask &= *flags;
            tracked = 0;
            *flags = mask;
        }
        SceneActorChase_AndWordFlags(&actor->base.behaviorFlags, ~0x80);
        state->phase = tracked;
        tracking = &actor->base.tracking;
        tracked = SceneActorChase_TrackingFlags(tracking, 8);
        if (tracked) {
            SceneActorReaction_EnableMotionFlags(0x100, flags);
            SceneActorControl_StartPositionCurve(actor, &gPlayers[0]->position);
            SelectState(actor, 0, 2, 0);
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
            SceneActorControl_ResetCollisionPhase(actor);
        } else {
            OrTrackingFlags(tracking, 8);
            SceneActorReaction_EnableMotionFlags(0x100, flags);
            SelectState(actor, 0, 0, 3);
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
            SceneActorControl_ResetCollisionPhase(actor);
        }
    }
}
