#define SCENE_ACTOR_CHASE_WIDE_MAP_WIDTH           1
#define SCENE_ACTOR_CHASE_FRACTION_ACCESSOR        1
#define SCENE_ACTOR_CHASE_DIRECT_SAVED_CONTROL     1
#include "scene_actor_displacement.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "spark_particle.h"

void sub_08087018(struct ChaseActor *, s32);

static inline u8 Bat_DisplacementStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

/* Ease recoil displacement, then select route or pursuit recovery. */
void sub_08087A70(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *animation =
        (struct DisplaySequenceState *)((struct SceneActorAnimationSequenceDrawable *)
                                            actor->base.drawable)
            ->sequence;
    struct ContactPackedVector range;
    struct SceneActorChaseGroup *group = &actor->group;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)actor, 3);
        if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
            Sound_PlaySong(0xD4);
            Sound_PlaySong(0x12A);
        }
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
        actor->base.restart = 0;
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10000;
        actor->base.flags2D0 |= mask;
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
        SparkParticle_Spawn(&actor->base.body.position, count,
                            (const struct ContactPosition *)&range,
                            actor->base.body.direction + 128, 10, 6, 2, 2);
    }
    dx = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.x, &actor->base.reactionEnd.x,
                                         &x);
    dz = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.z, &actor->base.reactionEnd.z,
                                         &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->base.body.motion.x += dx;
    actor->base.body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (!Bat_DisplacementStopped(dx, dz))
        return;
    if (!actor->base.restart)
        return;
    actor->base.field2B0 = 1;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x120);
    SceneActorChase_ClearActorFlags(actor, 0x80);
    if (actor->base.field2DC & 0x80000) {
        SceneActorChase_SetReactionFlags(actor, 2);
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 8)) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, sub_08087018, 0x14);
        SceneActorChase_ClearMotionFlagComponent(actor, 8);
        actor->base.field2C2 = 64;
        return;
    }
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&group->common.gate.flags,
                                         4);
        SceneActorChase_TrackPlayer(actor);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
        return;
    }
    SceneActorChase_TrackPlayer(actor);
    SceneActorChase_SetActorFlags(actor, 0x10);
    group->destination->pad = 0;
    if (!SceneActorChase_Arrive(actor)) {
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      &actor->base.tracking.target->position)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
    SceneActorChase_SetControl(actor, actor->base.entryTarget228, 1);
    SceneActorChase_SetBehavior(actor, 7);
    actor->base.field2C2 = 32;
}
