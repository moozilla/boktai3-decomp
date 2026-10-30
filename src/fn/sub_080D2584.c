#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH           1
#define SCENE_ACTOR_CHASE_FRACTION_ACCESSOR        1
#define SCENE_ACTOR_CHASE_DIRECT_ROUTE_FIELDS      1
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL     1
#define SCENE_ACTOR_CHASE_SCALAR_COORDINATE_OUTPUT 1
#define SCENE_ACTOR_CHASE_ROUTE_COUNT_ACCESSOR     1
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "spark_particle.h"

void SceneActorMovement_ResolveRoutePosition(void *);
void Mummy_ResumeRouteAfterRecovery(struct SceneActorRegistryResetView *, s32);

static inline s32 Mummy_QuarterDisplacement(s32 value)
{
    return value >= 0 ? value >> 2 : -(-value >> 2);
}

static inline u8 Mummy_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline s32 Mummy_ReadDisplacementAxis(s32 desired, s16 *current, s16 **field)
{
    s32 difference = desired - *current;
    *field = current;
    return Mummy_QuarterDisplacement(difference);
}

/* Ease recoil displacement, then select route or pursuit recovery. */
void Mummy_UpdateDisplacementRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct DisplaySequenceState *animation =
        (struct DisplaySequenceState *)((struct SceneActorAnimationSequenceDrawable *)
                                            actor->drawable)
            ->sequence;
    struct ContactPackedVector range;
    struct SceneActorChaseGroup *group;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->secondaryMotion.x = Mummy_QuarterDisplacement(actor->secondaryMotion.x);
        actor->secondaryMotion.z = Mummy_QuarterDisplacement(actor->secondaryMotion.z);
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)actor, 3);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
            Sound_PlaySong(0x12A);
            Sound_PlaySong(0xFD);
        }
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
        actor->field2B3 = 0;
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    if (frame <= 17 && animation->index == 0 && animation->elapsed == (animation->duration != 0) &&
        animation->duration != 1)
        animation->elapsed = 0;
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
    dx = Mummy_ReadDisplacementAxis(actor->reactionStart.x, &actor->reactionEnd.x, &x);
    dz = Mummy_ReadDisplacementAxis(actor->reactionStart.z, &actor->reactionEnd.z, &z);
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
    if (!Mummy_DisplacementStopped(dx, dz))
        return;
    if (!actor->field2B3)
        return;
    group = &actor->group;
    actor->field2B0 = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x10)) {
        SceneActorChase_ChooseRoute(actor);
        SceneActorMovement_ResolveRoutePosition(actor);
        SceneActorChase_SetBehavior(actor, 8);
        SceneActorChase_SetControl(actor, group->routeChoiceControl, 0x13);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        return;
    }
    if (actor->field2DC & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, Mummy_ResumeRouteAfterRecovery, 0x14);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        actor->field2C2 = 64;
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          &actor->tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return;
    }
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    SceneActorChase_TrackPlayer(actor);
    if (!SceneActorChase_Arrive(actor)) {
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      &actor->tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
    SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->field2C2 = Div(64, 3);
}
