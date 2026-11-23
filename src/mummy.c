#include "global.h"
#include "scene_actor_reset.h"
#include "mummy.h"
#include "scene_actor_animation.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_turn.h"
#include "display_sequence.h"
#include "scene_actor_chase.h"
#include "scene_actor_chase_shared.h"

/* Records attack contacts and queues a timed reaction (tentative). */

static inline u32 Mummy_Flag(u8 *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)(a + off) & m : *(u32 *)(a + off) & m;
}

static inline u8 Mummy_IsAttackBlocked(u8 *c)
{
    if (c && (Mummy_Flag(c, 0x40, 0x2D4) || Mummy_Flag(c, 0x20000, 0x2D0) ||
              ((struct ChaseActor *)c)->base.health <= 0))
        return 1;
    return 0;
}

void *SceneActorContacts_FindObjectById(u16);

static inline u8 Mummy_LoadContactLatch(struct Mummy *state, u16 **saved, u32 mask)
{
    u16 *p = &state->flags6F2;
    u32 v = *p & mask;
    *saved = p;
    if (v)
        return 1;
    return 0;
}

static inline u32 Mummy_ResponseAddress(u32 k_, void *base)
{
    return (u32)base + k_;
}

void Mummy_OnAttackCollision(u8 *unused, struct ContactCollider *obj, u8 *input)
{
    struct ChaseActor *base = (void *)input;
    struct Mummy *state = ((struct ChaseActor *)input)->base.context;
    u16 responseCost;
    u16 contactFlags;
    u32 rawKind;
    u32 kindIndex;
    if (Mummy_IsAttackBlocked(input))
        return;
    rawKind = obj->contactFlags;
    kindIndex = (u16)(rawKind - 1);
    contactFlags = rawKind;
    if (kindIndex <= 7) {
        u16 *flags;
        if ((!Mummy_LoadContactLatch(state, &flags, 0x800)) &&
            SceneActorChase_Bits32(&((struct Player *)obj->owner)->statusFlags, 8))
            return;
        {
            u32 mask = ~0x800;
            *flags &= mask;
        }
        if ((!Mummy_IsAttackBlocked(input)) && ((obj->contactFlags) & 15)) {
            ((struct ChaseActor *)input)->base.counter304 = obj->collisionId;
            SceneActorChase_ClearWordFlags(&((struct ChaseActor *)input)->base.statusFlags,
                                           ROOM_ACTOR_STATUS_INTERACTING);
        }
        base->base.tracking.target =
            SceneActorContacts_FindObjectById(*(u16 *)&base->base.counter304);
        {
            u32 value = obj->responseFlags;
            u32 mask = 1;
            mask |= value;
            obj->responseFlags = mask;
        }
        if ((!(*((u16 *)Mummy_ResponseAddress(
                (u32) & ((struct ChaseActor *)0)->base.colliderA.responseValue, base)))) &&
            SceneActorChase_Bits32(&((struct Player *)obj->owner)->statusFlags, 0x200)) {
            base->base.hitOwners = 1;
            base->base.hitReactionFlags = 4;
            base->base.hitAttributes = 0;
            {
                struct ContactCollider *collider = &base->base.colliderA;
                collider->cost = 1;
            }
            responseCost = base->base.colliderA.cost;
            base->base.reactionSoundVariant = responseCost;
            base->base.colliderA.responseParameter = 1;
            base->base.colliderA.responseValue = 30;
            base->base.field2E6 = 4;
        }
    } else if ((!Mummy_IsAttackBlocked(input)) && (contactFlags & 15)) {
        ((struct ChaseActor *)input)->base.counter304 = obj->collisionId;
        SceneActorChase_ClearWordFlags(&((struct ChaseActor *)input)->base.statusFlags,
                                       ROOM_ACTOR_STATUS_INTERACTING);
    }
}

void SceneActorReaction_HandleColliderContact(void *, void *, void *);

void Mummy_OnBodyCollision(void *source, void *destination, void *actor)
{
    SceneActorReaction_HandleColliderContact(source, destination, actor);
}

struct SceneActorCheckpointSequence {
    u8 unknown0[8];
    u16 frame;
    u8 unknownA[4];
    u16 timer;
};

void Mummy_FireAcceleratingVolley(struct ChaseActor *);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void SceneActor_UpdateAnimationCheckpoint(struct ChaseActor *actor)
{
    struct Mummy *state = actor->base.context;
    struct SceneActorCheckpointSequence *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    if (TakeFlag(&actor->base.field2B2)) {
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        mask = 0x100;
        actor->base.motionFlags |= mask;
    }
    if (sequence->frame > 2) {
        if (sequence->frame == 4 && sequence->timer == 0)
            Mummy_FireAcceleratingVolley(actor);
    }
    if (actor->base.restart) {
        state->savedTick = state->currentTick;
        state->targetTick = state->currentTick + 30;
        actor->base.stepDone = 1;
    }
}

void Mummy_OnUnusedState(void)
{
}

void Actor_InitLaunchVelocity(struct ChaseActor *);

s32 Math_Atan2(s32, s32);

void SceneActor_UpdateDirectionalAnimation(struct ChaseActor *actor)
{
    struct SceneActorBody *body = &actor->base.body;
    struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
    struct DisplaySequenceState *sequence = &drawable->sequence;
    if (TakeFlag(&actor->base.field2B2)) {
        u32 mask;
        Actor_InitLaunchVelocity(actor);
        body->direction = Math_Atan2(actor->base.secondaryMotion.x, actor->base.secondaryMotion.z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        mask = 0x100;
        actor->base.motionFlags |= mask;
    }
    if (sequence->index > 2) {
        if (sequence->index == 4 && sequence->elapsed == 0)
            Sound_PlaySong(0x101);
    }
    if (SceneActorChase_StatusFlags(actor, 0x10)) {
        u16 *flags = &actor->base.motionFlags;
        u32 mask = ~0x100;
        mask &= *flags;
        *flags = mask;
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x2000);
    }
}

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void Mummy_UpdateWalking(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAttachment_ReplaceResponseSlot(p, 0, 0x7F, 0);
    SceneActorAnimation_SelectDirectionalEntry(p, 1);
}

struct S {
    u8 filler[0x2b2];
    u8 f;
};

void Mummy_UpdateIdle(struct S *s)
{
    if (s->f != 0)
        s->f = 0;
    SceneActorAnimation_SelectDirectionalEntry(s, 0);
}

u32 Route_GetCurrentPointFacing(void *);

static inline u16 MummyTurn_Elapsed(void *sequence)
{
    return ((struct DisplaySequenceState *)sequence)->elapsed;
}

/* Turn toward the route-facing octant, then restore the waiting controller. */

void Mummy_TurnTowardRoute(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group;
    struct SceneActorBody *motion = &a->base.body;
    void *sequence = &((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence;
    u16 *motionFlags;
    u8 prepared;
    s32 direction, target, delta;

    group = &a->group;
    prepared = SceneActorReaction_TakePrepared(a);
    motionFlags = &a->base.motionFlags;
    if (prepared) {
        motion->direction = (SceneActorTurn_Cardinal(motion->direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(motion->direction);
        if (a->base.behavior2AA != 6 && !(SceneActorTurn_Flags(motionFlags, 0x480))) {
            target = (((u16)Route_GetCurrentPointFacing(motion->currentRoute) + 0x70) >> 5) & 7;
            a->base.nextDirection = target;
        } else {
            target = a->base.nextDirection;
        }
        if (direction == (target >> 1)) {
            a->base.stepDone = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            }
            return;
        }
        a->base.nextDirection = target;
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 0, 0);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 2) > 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 0, 4);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x10000);
                }
                break;
        }
        a->base.restart = 0;
    }
    if (MummyTurn_Elapsed(sequence) == 0) {
        s32 targetDirection = SceneActorTurn_OctantDirection(a->base.nextDirection);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x15)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x15;
        else
            motion->direction -= 0x15;
    }
    SceneActorTurn_ApplyDuration(a, motionFlags);
    if (a->base.restart) {
        s32 current = ((motion->direction + 0x70) >> 5) & 7;
        delta = a->base.nextDirection - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->base.stepDone = 1;
            a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(a, a->base.onEntry, 2);
        }
    }
}

void Mummy_ResetMotionState(void *);

void Mummy_ResumeSavedRouteAfterAnimation(void *);

static inline void StoreHalf(struct ChaseActor *a, u32 v)
{
    u32 value = v;
    a->base.paletteTimer = value;
}

// confidence=low: waits for the recovery timer before restoring route animation.

void Mummy_UpdateRecoveryWait(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    u16 *speed;
    if (SceneActorReaction_TakePrepared(a)) {
        SceneActorAttachment_ReplaceResponseSlot(a, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(a, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(a, 3, 127, 0);
        SceneActorChase_SetMotionFlags(a, 16);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 1);
    a->base.field2C2 = Div(64, 6);
    speed = &a->base.field2C2;
    if (!group->field5C4) {
        a->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 6);
        Mummy_ResetMotionState(a);
        StoreHalf(a, 64);
        Chase_SetState(a, 3, 0, 0);
        Chase_SetCtl(a, (void *)Mummy_ResumeSavedRouteAfterAnimation, 0x13);
        {
            u16 *p = &a->base.motionFlags;
            s32 mask = ~0x30;
            *p &= mask;
        }
        SceneActorChase_SetActorFlags(a, 16);
        *speed = 64;
    }
}
