#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "dog.h"
#include "scene_actor_behavior.h"
#include "scene_actor_control_phase.h"
#include "script_utility.h"

s32 Mod(s32, s32);

void SceneActorAnimation_SpawnTrailAtPosition(void *, u32);

void SceneActorAnimation_RefreshTimedAttackCollider(struct ChaseActor *a, u32 frame);

static inline u8 TakePrepared(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

void SceneActorAnimation_EmitDirectionalTrail(struct ChaseActor *a, s32 frame)
{
    union SceneActorRegistryPackedPosition p;
    if (TakePrepared(a))
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 17);
    {
        struct DisplaySequenceState *sequence = (void *)((u8 *)a->base.drawable + 0x48);
        sequence->index = 0;
        sequence = (void *)((u8 *)a->base.drawable + 0x48);
        sequence->elapsed = 0;
    }
    if (Mod(frame, 4) == 0) {
        s32 direction = ((a->base.body.direction + 32) & 255) >> 6;
        p.position = a->base.body.position;
        switch (direction) {
            case 0:
                p.packed.x += 128;
                break;
            case 1:
                p.packed.z += 128;
                break;
            case 2:
                p.packed.x -= 160;
                break;
            case 3:
                p.packed.z -= 128;
                break;
        }
        SceneActorAnimation_SpawnTrailAtPosition(&p, a->base.paletteOffset);
    }
    if (frame > 30) {
        void *callback = SceneActorAnimation_RefreshTimedAttackCollider;
        u8 kind = 38;
        u8 *prepared = &a->base.field2B2;
        u32 zero = 0;
        *prepared = 1;
        a->base.stepDone = zero;
        a->base.controlKind = kind;
        a->base.target = callback;
        a->base.field2C8 = zero;
        {
            u32 *flags = &a->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        a->base.pendingControl = zero;
    }
}

struct SceneActorTimedAttackContext {
    u8 unknown0[0x624];
    struct ContactCollider collider;
};

void FeatherEffectPool_CreateDirectional(struct ContactPosition *, u32, u32, u32);

void SceneActorAnimation_RefreshTimedAttackCollider(struct ChaseActor *a, u32 frame)
{
    struct SceneActorTimedAttackContext *context = a->base.context;
    if (TakePrepared(a)) {
        u32 variant, palette;
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 17);
        palette = a->base.paletteOffset;
        variant = 0;
        if (palette == 1)
            variant = 1;
        FeatherEffectPool_CreateDirectional(&a->base.body.position, a->base.body.direction, 0,
                                            variant);
        Sound_PlaySong(0x3A5);
    }
    if (frame - 13 <= 10)
        Collider_Refresh(&context->collider);
    if (a->base.restart)
        a->base.stepDone = 1;
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void Dog_LeapTowardTarget(struct ChaseActor *actor);

static inline u8 TakePending(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void BeginControl(struct ChaseActor *actor, void *control, u8 kind)
{
    u32 zero;
    actor->base.field2B2 = 1;
    zero = 0;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}

void Dog_UpdateLeapWindup(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    if (TakePending(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(actor, 0xF);
        *(struct ContactPosition *)(context + 0x6D8) =
            *(struct ContactPosition *)(*(u8 **)((u8 *)actor + 0xDC) + 0xC);
    }
    if (actor->base.restart)
        BeginControl(actor, Dog_LeapTowardTarget, 0x23);
}

/* Dog_LeapTowardTarget: on entry turns the leap velocity toward the context's target position (snapping to it within 16, else turning 16 at a time) and plays sound 0x3A4; halves the velocity while the motion's bit 0 is set, and hands over to the landing reaction (kind 0x24) when flags2D0 & 0x10 (provisional name) */

void Dog_UpdateAnimationSixteenReaction(u8 *p);

s32 Math_Atan2(s32, s32);

extern const s16 gUnk_086149C4[];

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline s32 Half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}

static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xFF] * speed);
}

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void Dog_LeapTowardTarget(struct ChaseActor *actor)
{
    u8 *context = actor->base.context;
    struct ContactPosition velocity;
    if (TakeFlag(&actor->base.field2B2)) {
        s32 angle, facing, turn;
        context = actor->base.context;
        Delta(&velocity, &actor->base.body.position, (struct ContactPosition *)(context + 0x6D8));
        angle = Math_Atan2(velocity.x, velocity.z);
        facing = ((actor->base.body.direction + 0x20) & 0xFF) >> 6 << 6;
        turn = angle - facing;
        if (turn > 0x80)
            turn -= 0x100;
        else if (turn < -0x80)
            turn += 0x100;
        if ((turn < 0 ? -turn : turn) <= 0xF)
            Polar(&velocity, angle, 24);
        else if (turn > 0)
            Polar(&velocity, (facing + 0x10) & 0xFF, 24);
        else
            Polar(&velocity, (facing + 0xEF) & 0xFF, 24);
        actor->base.secondaryMotion.x = velocity.x;
        actor->base.secondaryMotion.y = 0x20;
        actor->base.secondaryMotion.z = velocity.z;
        Sound_PlaySong(0x3A4);
    }
    Collider_Refresh(context + 0x624);
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        actor->base.secondaryMotion.x = Half(actor->base.secondaryMotion.x);
        actor->base.secondaryMotion.z = Half(actor->base.secondaryMotion.z);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    {
        u32 mask = 0x2000;
        actor->base.behaviorFlags |= mask;
    }
    {
        u32 mask = 0x10;
        if (actor->base.statusFlags & mask) {
            void *callback = (void *)Dog_UpdateAnimationSixteenReaction;
            u32 kind = 0x24;
            actor->base.field2B2 = 1;
            actor->base.stepDone = 0;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = 0;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = 0;
        }
    }
}

void Dog_UpdateAnimationSixteenReaction(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0x10);
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}

void Dog_UpdateAnimationFourteenReaction(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 0xE);
    if (*(u8 *)(p + 0x2B3))
        *(u8 *)(p + 0x2B0) = 1;
}

static inline void Dog_SetTurnFlags(struct ChaseActor *actor, u16 mask)
{
    actor->base.motionFlags = mask | actor->base.motionFlags;
}

static inline void Dog_ResetAnimation(struct ChaseActor *actor, u32 state, u32 phase)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void Dog_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    u32 *flags;
    u32 mask;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    flags = &actor->base.pendingControlFlags;
    mask = ~1;
    *flags &= mask;
    actor->base.pendingControl = 0;
}

void Dog_UpdateTurnSequence(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Dog_SetTurnFlags(actor, 0x400);
        actor->base.field2C2 = 32;
    }
    if (group->turn.turnsRemaining > 0) {
        if (frame > 20) {
            /* data holds the entry control, then the random table. */
            const void *data;
            u32 encoded;
            Dog_ResetAnimation(actor, 3, 1);
            data = actor->base.onEntry;
            Dog_SetControl(actor, 2, (void *)data);
            if (group->turn.turnsRemaining == 1) {
                encoded = ((group->turn.savedDirection >> 5) + 3) & 7;
                actor->base.nextDirection = encoded;
            } else {
                s32 prior = actor->base.body.direction;
                s32 tries = 0;
                u32 *randomIndex;
                u32 wrap, mask;
                s32 choice;
                data = gRandomTable;
                randomIndex = &gRandomIndex;
                wrap = 1023;
                mask = 3;
                for (;;) {
                    u32 index = (*randomIndex + 1) & wrap;
                    s32 next;
                    *randomIndex = index;
                    next = (((const u16 *)data)[index] & mask) << 6;
                    if (next != prior) {
                        choice = next;
                        break;
                    }
                    if (++tries > 3) {
                        choice = prior;
                        break;
                    }
                }
                encoded = ((choice >> 5) + 3) & 7;
                actor->base.nextDirection = encoded;
            }
            group->turn.turnsRemaining--;
        }
    } else {
        group->turn.turnsRemaining |= -1;
        if (frame > 6) {
            struct SceneActorChaseSavedControl *saved = &group->saved;
            u32 kind = saved->kind;
            void *control = saved->control;
            u32 state, phase, behavior;
            Dog_SetControl(actor, kind, control);
            state = saved->state;
            phase = saved->phase;
            behavior = saved->behavior;
            actor->base.state2A8 = state;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
            {
                u16 *flags = &actor->base.motionFlags;
                u32 mask = ~0x400;
                *flags = mask & *flags;
            }
        }
    }
}

void Dog_OnSecondaryReactionCleanup(void)
{
}

#define SceneActorMotion_IsVariantTimerReady Dog_IsActionTimerExpired

s32 Dog_IsActionTimerExpired(struct ChaseActor *);

s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);

s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);

static inline u8 TestMovement(struct ChaseActor *a)
{
    void *collider = a->group.route;
    struct ContactPosition *position = &a->base.body.position;
    struct ContactPosition *destination = &a->base.tracking.target->position;
    if (!Nav_IsPositionInMesh(collider, position, destination)) {
        u16 *flags = &a->base.motionFlags;
        u32 mask = ~0x200;
        *flags &= mask;
        return 0;
    } else {
        s32 result = Terrain_ApplyBlocked(collider, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&a->base.body.motion, 8);
            return 1;
        } else {
            u16 *flags = &a->base.motionFlags;
            u32 mask = ~0x200;
            *flags &= mask;
            return 0;
        }
    }
}

void SceneActorMotion_TestTargetTransition(struct ChaseActor *a)
{
    struct Dog *context = a->base.context;
    if (!TakePrepared(a)) {
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 11);
        if ((u8)Dog_IsActionTimerExpired(a)) {
            u8 *tail = (u8 *)a + 0x3D4;
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 0);
            if (!TestMovement(a))
                Callback_InvokeActor(a, *(void **)(tail + 0x224));
            else {
                u32 *flags = &context->flags;
                s32 mask = -5;
                *flags &= mask;
                {
                    u32 next = 17;
                    u8 *state = &a->base.state2A8;
                    u32 zero = 0;
                    *state = zero;
                    *((u8 *)a + 0x2A9) = zero;
                    a->base.behavior2AA = next;
                    a->base.counter2AC = zero;
                    a->base.field2B1 = 1;
                }
            }
        }
    }
}

struct SceneActorTargetMotionContext {
    u8 unknown0[0x678];
    u32 flags;
    u8 unknown67C[0x10];
    u32 timeLimit;
    u8 unknown690[0x17];
    u8 phase;
};

s32 Dog_SelectRetreatDirection(struct ChaseActor *, u32);

static inline u8 IsActive(u32 flags)
{
    if (flags)
        return 1;
    return 0;
}

static inline void SetPhase(struct ChaseActor *a, u32 phase)
{
    u8 *state = &a->base.state2A8;
    u32 zero = 0;
    *state = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = phase;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}

void SceneActorMotion_UpdateTargetTransition(struct ChaseActor *a)
{
    u32 *flags;
    struct SceneActorTargetMotionContext *c = a->base.context;
    if (!TakePrepared(a)) {
        u32 four = 4;
        u32 *address = &c->flags;
        u32 value = *address & four;
        flags = address;
        if (IsActive(value)) {
            if (a->base.tracking.distance > c->timeLimit) {
                SetPhase(a, 19);
                {
                    s32 angle = a->base.tracking.heading;
                    if (angle >= 0)
                        angle >>= 5;
                    else
                        angle = -(-angle >> 5);
                    a->base.nextDirection = (angle + 3) & 7;
                }
            } else {
                s32 mask = -5;
                *flags &= mask;
                if (c->phase <= 3) {
                    if ((u8)Dog_SelectRetreatDirection(a, a->base.body.direction))
                        SetPhase(a, 17);
                } else {
                    SetPhase(a, 19);
                    {
                        s32 angle = a->base.tracking.heading;
                        if (angle >= 0)
                            angle >>= 5;
                        else
                            angle = -(-angle >> 5);
                        a->base.nextDirection = (angle + 3) & 7;
                    }
                }
            }
        } else
            SceneActorAnimation_SelectDirectionalEntry(&a->base, 1);
    }
}

/* SceneActorControl_BeginHiddenDefeat: on entry faces forward, releases attachments, plays the category sound and clears the defeat flags; once field2B3 is set either completes the defeat at once (flags2CC & 0x400: scripts, event flag, idle, hide) or hands over to the blink-out (SceneActorControl_CompleteBlinkOut, kind 0x27) (provisional name) */

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void SceneActorEffects_AdvanceSteps(void *, u32);

void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);

void SceneActorControl_CompleteBlinkOut(struct ChaseActor *, s32);

void SceneActorMovement_UpdateAirborne(struct ChaseActor *);

void SceneActorMovement_UpdateHeight(struct ChaseActor *);

void SceneActorControl_CallContextScript(struct ChaseActor *);

static inline void SetAnimation(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void Hide(u8 *sprite, u32 mask)
{
    *(u32 *)(sprite + 8) |= mask;
}

void SceneActorControl_BeginHiddenDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u16 *flags;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct SceneActorRegistryControl *control = &actor->group.common.control;
            u32 fieldOffset = 0x2CC;
            u32 *field;
            u32 mask;
            u32 zero;
            field = (void *)((u8 *)actor + fieldOffset);
            mask = ~0x141C0;
            *field &= mask;
            zero = 0;
            control->counter = zero;
            control->field14 = zero;
            control->timer = zero;
        }
        {
            u16 *colliderFlags = (u16 *)((u8 *)actor + 0x13A);
            u32 mask;
            u32 value = *colliderFlags;
            mask = 4;
            mask |= value;
            *colliderFlags = mask;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (!actor->base.restart)
        return;
    {
        u32 mask = 0x400;
        u32 instant = actor->base.behaviorFlags & mask;
        if (instant) {
            actor->base.stepDone = 1;
            SceneActorControl_CallContextScript(actor);
            script = (u16)actor->base.script1F8;
            if (script) {
                values[0] = actor->base.body.position.x;
                values[1] = actor->base.body.position.y;
                values[2] = actor->base.body.position.z;
                values[3] = actor->base.parameters1FC[0];
                values[4] = actor->base.parameters1FC[1];
                values[5] = actor->base.parameters1FC[2];
                values[6] = actor->base.hitOwners;
                values[7] = actor->base.body.id;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(script, &arguments);
            }
            {
                u32 mask = 0x4000;
                u16 value = actor->base.motionFlags;
                u16 test = value & mask;
                flags = &actor->base.motionFlags;
                if (!test && (u32)(actor->base.category - 11) > 2)
                    Bestiary_UnlockEntry(
                        Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
            }
            {
                u32 mask = 0x4000;
                u16 value = *flags;
                u16 busy = value & mask;
                if (!busy) {
                    void *callback = (void *)SceneActorControl_OnIdle;
                    u32 kind;
                    kind = 16;
                    actor->base.field2B2 = 1;
                    actor->base.stepDone = 0;
                    actor->base.controlKind = kind;
                    actor->base.target = callback;
                    actor->base.field2C8 = 0;
                    {
                        u32 *flags = &actor->base.pendingControlFlags;
                        u32 mask = ~1;
                        *flags &= mask;
                    }
                    actor->base.pendingControl = 0;
                    SetAnimation(actor, 1, 0, 2);
                }
            }
            ActorShadow_Disable((u8 *)actor + 0x320);
            {
                u32 mask = 1;
                if (!actor->base.drawableKind)
                    *(u32 *)actor->base.drawable |= mask;
                else
                    Hide((u8 *)actor->base.drawable + 0x20, mask);
            }
        } else {
            void *callback = (void *)SceneActorControl_CompleteBlinkOut;
            u32 kind;
            kind = 0x27;
            actor->base.field2B2 = 1;
            actor->base.stepDone = instant;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = instant;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = instant;
        }
    }
}
