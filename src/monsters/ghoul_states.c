#include "monsters/ghoul_disc.h"
#include "monsters/ghoul.h"

#include "scene/actor/scene_actor_availability.h"

#include "global.h"

#include "scene/actor/scene_actor_chase_shared.h"

#include "monsters/scene_actor_chase_step.h"

#include "scene/actor/scene_actor_reaction.h"

#include "monsters/scene_actor_init_primitives.h"

#include "engine/video/display_sequence.h"

#include "scene/actor/scene_actor_chase_reaction.h"

#include "scene/actor/scene_actor_turn.h"
#include "scene/actor/scene_actor_status.h"

void Ghoul_OnAttackCollision(struct ContactCollider *unused, struct ContactCollider *contact,
                             struct ChaseActor *actor)
{
    int mask;
    u32 *flags;
    if ((SceneActor_ActionsBlocked((void *)actor) == 0) && (contact->contactFlags & 0xF)) {
        actor->base.counter304 = contact->collisionId;
        flags = &actor->base.statusFlags;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void SceneActorReaction_HandleColliderContact(void *, void *, void *);

void Ghoul_OnBodyCollision(struct ContactCollider *source, struct ContactCollider *destination,
                           struct ChaseActor *actor)
{
    SceneActorReaction_HandleColliderContact(source, destination, actor);
}

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

void SunDiscPool_SpawnDisc(const struct ContactPosition *, const struct ContactPosition *, s32, s32,
                           u32, s32, u32, s32, s32);

void Ghoul_UpdateAimedDiscThrow(struct ChaseActor *actor)
{
    struct Ghoul *context = actor->base.context;
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.body.direction = (((actor->base.tracking.heading + 32) & 255) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        if (Ghoul_TestDiscFlags(context, 4)) {
            sequence->index = 2;
            sequence->elapsed = 0;
            Ghoul_ClearDiscFlags(context, 4);
        }
        Ghoul_CaptureTarget(actor, context, 0);
    }
    if (sequence->index == 5 && sequence->elapsed == 0) {
        s32 direction = (((actor->base.body.direction + 32) & 255) >> 6) << 6;
        s32 delta = actor->base.tracking.heading - direction;
        struct ContactPosition origin;
        if (delta > 128)
            delta -= 256;
        else if (delta < -128)
            delta += 256;
        if (Ghoul_Abs(delta) < 32) {
            Ghoul_CaptureTarget(actor, context, 1);
        } else {
            s32 cell = Ghoul_ForwardCell(actor, 1);
            SceneActorTerrain_GetCenter(cell, &context->discTargets[1]);
            {
                switch (delta = Ghoul_QuarterDirection(actor->base.body.direction + 32) & 3) {
                    case 0:
                        context->discTargets[1].x += 127;
                        context->discTargets[1].z = actor->base.body.position.z;
                        break;
                    case 1:
                        context->discTargets[1].x = actor->base.body.position.x;
                        context->discTargets[1].z += 127;
                        break;
                    case 2:
                        context->discTargets[1].x -= 127;
                        context->discTargets[1].z = actor->base.body.position.z;
                        break;
                    case 3:
                        context->discTargets[1].x = actor->base.body.position.x;
                        context->discTargets[1].z -= 127;
                        break;
                }
                {
                    struct ContactPosition *position = &context->discTargets[1];
                    context->discTargets[1].y =
                        SceneActorChase_HeightAt(MonsterInit_CellIndex(position), position);
                }
            }
        }
        origin = actor->base.body.position;
        origin.y += 220;
        {
            u32 first = actor->base.attacks.first[1];
            u32 second = actor->base.attacks.second[1];
            u32 flags = actor->base.attacks.flags[1];
            u32 parameter = actor->base.attacks.parameter[1];
            SunDiscPool_SpawnDisc(&origin, &context->discTargets[1], first, second, flags,
                                  parameter, context->discKind, 30, -20);
        }
        Sound_PlaySong(0xD6);
    }
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

void Ghoul_UpdateIdleState(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
}

void Ghoul_UpdateStandingState(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
}

u32 Route_GetCurrentPointFacing(void *);

/* Turn toward the route-facing octant, then restore the waiting controller. */

void Ghoul_TurnTowardRoute(struct ChaseActor *actor)
{
    struct SceneActorBody *motion = &actor->base.body;
    struct SceneActorRegistryRoute *route = &actor->group.homeRoute;
    struct SceneActorChaseGroup *group = &actor->group;
    struct DisplaySequenceState *sequence;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(actor);
    s32 direction, target, delta;

    motionFlags = &actor->base.motionFlags;
    if (prepared) {
        actor->base.body.direction =
            (SceneActorTurn_Cardinal(actor->base.body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(actor->base.body.direction);
        if (actor->base.behavior2AA != 6 && actor->base.behavior2AA != 17 &&
            !SceneActorTurn_Flags(motionFlags, 0x4480)) {
            target = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            actor->base.nextDirection = target;
        } else {
            target = actor->base.nextDirection;
        }
        if (direction == (target >> 1)) {
            actor->base.stepDone = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
                SceneActorChase_SetState(actor, 3, 1, 0);
                SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 0, 4);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 9, 2, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 4);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 5);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 2, 4);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(actor, 9, 0, 0);
                    SceneActorChase_SetActorFlags(actor, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(actor, 9, 1, 1);
                    SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
                }
                break;
        }
        actor->base.restart = 0;
    }
    sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (sequence->elapsed == 0 && (sequence->index == 1 || sequence->index == 3)) {
        s32 targetDirection = SceneActorTurn_OctantDirection(actor->base.nextDirection);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x20)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(actor, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    SceneActorTurn_ApplyDuration(actor, motionFlags);
    if (actor->base.restart) {
        s32 current = ((actor->base.body.direction + 0x70) >> 5) & 7;
        delta = actor->base.nextDirection - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            actor->base.stepDone = 1;
            actor->base.body.direction =
                (SceneActorTurn_Cardinal(actor->base.body.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(actor, 3, 1, 0);
                SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(actor, actor->base.onEntry, 2);
        }
    }
}

void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, u32);

void Ghoul_UpdateDyingState(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 blockingFlags;
    u32 mask;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 8);
        test = 2;
        flags = &actor->base.motionFlags;
        value = *flags;
        test &= value;
        if (test == 0) {
            test = 1;
            test |= value;
            *flags = test;
        }
    }
    if (Mod(frame, 60) == 0) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 2, 0, actor->base.category);
    }
    if ((s16)group->field5C6 <= 0) {
        mask = 0x80;
        mask <<= 5;
        blockingFlags = actor->base.statusFlags & mask;
        if (blockingFlags == 0) {
            SceneActorChase_SetControl(actor, Ghoul_UpdateDeathAnimation, 6);
        }
    }
}

void Ghoul_UpdateDeathAnimation(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
        SceneActorChase_ClearMotionFlagComponent(actor, 1);
    }
    if (frame == 0x28)
        SceneActorChase_SetReactionFlags(actor, 0x10);
    if (actor->base.restart && frame > 0x78)
        actor->base.stepDone = 1;
}

void Bat_RestoreSavedState(struct ChaseActor *);

void Ghoul_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame, u32 unused)
{
    u32 queued = SceneActorReaction_TakePrepared(actor);
    if (queued) {
        ActorStatus_SetDrawableFlags(actor, 1);
        ActorShadow_Disable(actor->base.shadow);
    } else if (frame > 0x1A) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 9);
        SceneActorChase_SetControl(actor, Bat_RestoreSavedState, 0xD);
    } else {
        actor->base.restart = queued;
    }
}
