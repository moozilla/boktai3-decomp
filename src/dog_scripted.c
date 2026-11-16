#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"
#include "dog.h"
#include "scene_actor_chase.h"

void Dog_UpdateArrivalAnimation(void);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void SceneActorBehavior_CallActionScript(u8 *);

void Dog_SelectTargetAction(u8 *);

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

struct S {
    u8 pad0[0x59];
    u8 at59;
    u8 pad1[0x98 - 0x5A];
    u8 at98;
    u8 pad2[0xE9 - 0x99];
    u8 atE9;
    u8 pad3[0x104 - 0xEA];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6[0x2AC - 0x220];
    u32 counter;
    u8 done;
    u8 queued;
    u8 taken;
    u8 pad7[2];
    u8 code;
    u8 pad8[0x2C8 - 0x2B6];
    u16 at2C8;
    u8 pad9[0x2D4 - 0x2CA];
    u16 flags2D4;
    u8 pad10[0x414 - 0x2D6];
    u16 flags414;
    u16 at416;
    u16 at418;
};

void Dog_UpdateScriptedAction(struct S *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = Dog_UpdateArrivalAnimation;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->taken;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            obj->callback = fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
        {
            u32 byte = obj->atE9;
            obj->at59 = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->at98);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    }
    if (obj->done) {
        Dog_SelectTargetAction((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

static inline u32 Dog_RouteFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.behaviorFlags & mask;
}

/* Queue pursuit, a home route, or the short waiting response after the delay. */

void Dog_UpdateDelayedPursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    group = &actor->group;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->base.category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
        SceneActorChase_Arrive(actor);
    } else if (Dog_RouteFlags(actor, 0x10)) {
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    } else {
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 2, 30);
        actor->base.field2C2 = 64;
    }
    return;
waiting:
    actor->base.counter2AC++;
}

extern const u8 gUnk_08606204[];

struct SSetPrimaryBehaviorPreset {
    u8 filler[648];
    const u8 *p;
};

void Dog_SetPrimaryBehaviorPreset(struct SSetPrimaryBehaviorPreset *s)
{
    s->p = gUnk_08606204;
}

void SceneActor_ConfigureSecondaryProfileEntry(void *);

void SceneActorControl_TickActionCooldown(void *);

s32 Dog_PrepareFrame(void *p)
{
    SceneActor_ConfigureSecondaryProfileEntry(p);
    SceneActorControl_TickActionCooldown(p);
    return 1;
}

void Dog_UpdateCollisionBounds(void *);

void Dog_UpdateActionTimers(void *);

void Dog_UpdateActionCooldown(void *);

void Dog_TickCooldown(void *);

s32 Dog_FinishFrame(void *p)
{
    Dog_UpdateCollisionBounds(p);
    Dog_UpdateActionTimers(p);
    Dog_UpdateActionCooldown(p);
    Dog_TickCooldown(p);
    return 1;
}

#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))

void Dog_UpdateLeapWindup(void);

void Dog_SelectRetreatOrChase(struct ChaseActor *);

static inline void And32(u32 *p, u32 mask)
{
    *p &= mask;
}

static inline void Or16(u16 *p, u32 mask)
{
    u32 value = mask;
    value |= *p;
    *p = value;
}

static inline void Dog_MaskAttackFlags(u32 *field, u32 mask)
{
    *field &= mask;
}

static inline void Dog_ClearAttackFlags(struct Dog *ctx, u32 bits)
{
    Dog_MaskAttackFlags(&ctx->flags, ~bits);
}

void Dog_UpdateLeapAttack(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        {
            void *callback = Dog_UpdateLeapWindup;
            u32 kind = 34;
            a->base.field2B2 = 1;
            a->base.stepDone = 0;
            a->base.controlKind = kind;
            a->base.target = callback;
            a->base.field2C8 = 0;
            {
                u32 *flags = &a->base.pendingControlFlags;
                s32 mask = -2;
                *flags &= mask;
            }
            a->base.pendingControl = 0;
        }
        Dog_ClearAttackFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[1];
            u32 response = a->base.attacks.second[1];
            u32 power = a->base.attacks.flags[1];
            u32 extra = a->base.attacks.parameter[1];
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = 0;
        context->nearTimer = context->nearDistance;
    }
    if (a->base.stepDone) {
        context->nearTimer = context->nearDistance;
        {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->base.counter2AC++;
}

void SceneActorAnimation_EmitDirectionalTrail(void);

static inline void Dog_SetContextFlags(struct Dog *context, u32 mask)
{
    context->flags |= mask;
}

void Dog_UpdateTrailAttack(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (Chase_TakeArrived(a)) {
        And32(&context->flags, ~8);
        SceneActorReaction_EnableMotionFlags(0x100, &a->base.motionFlags);
        {
            void *callback = SceneActorAnimation_EmitDirectionalTrail;
            u32 kind = 37;
            a->base.field2B2 = 1;
            a->base.stepDone = 0;
            a->base.controlKind = kind;
            a->base.target = callback;
            a->base.field2C8 = 0;
            {
                u32 *flags = &a->base.pendingControlFlags;
                s32 mask = -2;
                *flags &= mask;
            }
            a->base.pendingControl = 0;
        }
        Dog_SetContextFlags(a->base.context, 2);
        {
            void *collider = &context->collider;
            u32 damage = a->base.attacks.first[2];
            u32 response = a->base.attacks.second[2];
            u8 *p = (u8 *)a + 0xD4;
            u32 power = *(u32 *)p;
            u32 extra;
            p -= 12;
            extra = *(u16 *)p;
            Collider_SetAttackProperties(collider, damage, response, 0, power, extra);
        }
        context->emitted = 0;
    }
    if (a->base.stepDone) {
        context->farTimer = context->farDistance;
        {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
        Dog_SelectRetreatOrChase(a);
    } else
        a->base.counter2AC++;
}

extern const u8 gUnk_0860620C[];

struct SSetSecondaryBehaviorPreset {
    u8 filler[652];
    const u8 *p;
};

void Dog_SetSecondaryBehaviorPreset(struct SSetSecondaryBehaviorPreset *s)
{
    s->p = gUnk_0860620C;
}

// confidence=low: tracks player zero and switches from the chase animation to a bite in range.

/* Dog_ChasePlayer: on entry starts the chase animation (kind 0x28); bites when a bite frame of the run cycle ends with player 0 in range (records the player's position, extends the chase timer), otherwise once the control finishes either follows the group's chase (flags2D4 & 0x80) or routes back to its route point (provisional name) */

void Dog_UpdateAnimationFourteenReaction(void);

u8 sub_0812F3CC(struct ChaseActor *);

struct DogAnim {
    u8 unknown0[7];
    u8 rate;
    u16 frame;
    u8 unknownA[4];
    u16 timer;
};

static inline u16 LoadMotionFlags(struct ChaseActor *actor, u32 mask)
{
    u16 value = actor->base.motionFlags;
    return value & mask;
}

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

void Dog_ChasePlayer(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    u8 *group = (u8 *)&actor->group.common;
    struct ContactPosition target;
    if (Chase_TakeArrived(actor))
        Chase_SetCtl(actor, Dog_UpdateAnimationFourteenReaction, 0x28);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    {
        struct DogAnim *anim;
        s32 distance;
        Delta(&target, &actor->base.body.position,
              (struct ContactPosition *)((u8 *)gPlayers[0] + 0x30));
        distance = target.x * target.x + target.z * target.z;
        anim = (struct DogAnim *)((u8 *)actor->base.drawable + 0x48);
        if ((anim->frame == 6 || anim->frame == 0xC || anim->frame == 0x13 ||
             anim->frame == 0x19) &&
            anim->timer == anim->rate - 1 && (u32)distance < *(u32 *)(context + 0x684)) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
            Chase_SetBehavior(actor, 0x10);
            *(struct ContactPosition *)(context + 0x6D8) =
                *(struct ContactPosition *)((u8 *)gPlayers[0] + 0x30);
            {
                u32 chasing = 0x80;
                if (actor->base.motionFlags & chasing) {
                    u16 *time = &actor->group.common.gate.counter;
                    *time += 0x12C;
                    if (*time > actor->group.common.gate.duration)
                        *time = actor->group.common.gate.duration;
                }
            }
            return;
        }
    }
    if (actor->base.stepDone) {
        if (LoadMotionFlags(actor, 0x80)) {
            Chase_SetBehavior(actor, 0xC);
            Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
            {
                u16 *field = (u16 *)(group + 0x40);
                u32 mask = 4;
                mask |= *field;
                *field = mask;
            }
            return;
        }
        Chase_SnapToRoute(&target, group + 0x74);
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 2);
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->base.body.position, &target))
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
    } else
        actor->base.counter2AC++;
}

// confidence=low: retained provisional behavior name from caller and state-transition evidence.

// confidence=low: resumes group chase or the saved route after arrival.

static inline u32 Dog_SetMotionBits(struct ChaseActor *actor, s32 mask)
{
    struct ChaseFlags40 *flags = (void *)&actor->base.motionFlags;
    flags->bits |= mask;
    return Chase_MotionFlags(actor, 0x80);
}

static inline void Dog_SetMovementBits(struct ChaseActor *actor, u32 mask)
{
    u32 *flags = &actor->base.behaviorFlags;
    *flags |= mask;
}

void Dog_ResumeRoute(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    u8 *context = a->base.context;
    if (Chase_TakeArrived(a))
        Chase_SetCtl(a, Dog_UpdateArrivalAnimation, 10);
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->base.stepDone) {
        if (!Chase_TryRoute(a, *(void **)(group + 0x1FC), &a->base.body.position,
                            (struct ContactPosition *)(context + 0x6D8))) {
            Callback_InvokeActor(a, *(void **)(group + 0x224));
            return;
        }
        if (Dog_SetMotionBits(a, 0x200)) {
            Chase_SetBehavior(a, 12);
            Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
            {
                u16 *flags = (u16 *)(group + 0x40);
                u32 mask = 4;
                mask |= *flags;
                *flags = mask;
            }
        } else {
            struct ContactPosition *p = *(struct ContactPosition **)(group + 0x200);
            Chase_SnapToRoute(p, group + 0x74);
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
            Dog_SetMovementBits(a, 0x10);
            Chase_SetCtl(a, a->base.onAlternateEntry, 1);
            Chase_SetBehavior(a, 7);
            a->base.field2C2 = 0x20;
        }
    } else
        a->base.counter2AC++;
}
