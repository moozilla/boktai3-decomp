#define SCENE_ACTOR_CHASE_DIRECT_ROUTE_FIELDS 1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "spark_particle.h"

u8 Skeleton_RerouteToTarget(void *);
u8 SceneActor_DispatchContextAction(void *, u32);
void Vorn_ResumeRouteAfterDelay(struct SceneActorRegistryResetView *, s32);

struct VornAnimationState {
    u8 unknown0[12];
    u16 delay;
};
struct VornDrawable {
    u8 unknown0[0x20];
    struct VornAnimationState animation;
};
static inline s32 Vorn_QuarterDisplacement(s32 value)
{
    return value >= 0 ? value >> 2 : -(-value >> 2);
}
static inline s32 Vorn_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Vorn_QuarterDisplacement(difference);
}
static inline u8 Vorn_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u8 Vorn_ContextFlags(struct SceneActorAnimationLink *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}
static inline void Vorn_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 = mask | actor->flags2D0;
}

#define Vorn_UpdateStaggerRecovery sub_080AD75C
void Vorn_UpdateStaggerRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorAnimationLink *context = actor->context;
    struct VornAnimationState *animation = &((struct VornDrawable *)actor->drawable)->animation;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(actor);
    struct ContactPackedVector range;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
            Sound_PlaySong(0x1CB);
            Sound_PlaySong(0x12A);
        }
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
        actor->field2B3 = 0;
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    Vorn_SetStatusFlags(actor, 0x10000);
    if (frame <= 17)
        animation->delay = 0;
    if (Mod(frame, 12) == 0 && SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        u32 index, sample;
        u8 count;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        index *= 2;
        sample = *(const u16 *)(RANDOM_TABLE_ADDR + index);
        sample &= 3;
        count = sample + 1;
        SparkParticle_Spawn(&actor->body.position, count, (const struct ContactPosition *)&range,
                            actor->body.direction + 128, 10, 6, 2, 2);
    }
    dx = Vorn_ReadDisplacementAxis(actor->reactionStart.x, &actor->reactionEnd.x, &x);
    dz = Vorn_ReadDisplacementAxis(actor->reactionStart.z, &actor->reactionEnd.z, &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Vorn_DisplacementStopped(dx, dz))
        return;
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (actor->field2DC & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, Vorn_ResumeRouteAfterDelay, 20);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        actor->field2C2 = 64;
    } else if (Vorn_ContextFlags(context, 0x1D))
        SceneActor_DispatchContextAction(context, 1);
    else {
        SceneActorChase_TrackPlayer(actor);
        SceneActorChase_SetActorFlags(actor, 16);
        group->destination->pad = 0;
        if (!Skeleton_RerouteToTarget(actor) &&
            !SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        SceneActorChase_SetBehavior(actor, 7);
        actor->field2C2 = 32;
    }
}
