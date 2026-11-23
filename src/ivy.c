#include "global.h"
#include "scene_actor_reset.h"
#include "ivy_phase.h"
#include "ivy.h"
#include "phase_object.h"
#include "word_flags.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "display_sequence_inline.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "gba/macro.h"
#include "gba/syscall.h"
#include "player.h"
#include "scene_actor_control_phase.h"
#include "game_state.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "runtime.h"
#include "scene_actor_part_burst.h"
#include "debug.h"
#include "scene_actor_chase.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "scene_actor_registry.h"
#include "scene_actor_state.h"
#include "scene_actor_drawable_flags.h"
#include "scene_body_api.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_group.h"
#include "render.h"
#include "engine_globals.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_actor.h"

void SceneActorBehavior_ApplyContactReaction(void *, void *, void *);

static inline u32 ActorFlag(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}

static inline u8 IsUnavailable(struct ChaseActor *actor)
{
    if (actor && (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) ||
                  actor->base.health <= 0))
        return 1;
    return 0;
}

static inline u32 ContextFlag(struct IvyCaptureState *context, u32 input)
{
    return context->captureFlags & input;
}

void Ivy_OnPrimaryContact(void *source, void *target, struct ChaseActor *actor)
{
    if (IsUnavailable(actor))
        return;
    if (SceneActorChase_Test(ContextFlag(actor->base.context, 1), ~0))
        return;
    SceneActorBehavior_ApplyContactReaction(source, target, actor);
}

void Ivy_OnSecondaryContact(void)
{
}

void SceneActorAnimation_SelectDirectionalFlags(struct RoomActor *, s32);

void Ivy_UpdateActiveState(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        u16 *q;
        ((u8 *)actor->base.drawable)[7] = 2;
        q = &actor->base.colliderA.flags;
        *q = (*q & 0xFFFB) | 8;
        HalfwordFlags_Clear(&actor->base.placementFlags, 0x800);
    }
    SceneActorAnimation_SelectDirectionalFlags(&actor->base, 0xE);
}

/* Tentative name based on the observed state transition or initialization. */

void Sound_PlaySong(u32);

void SceneActorReaction_PlayHiddenCue(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u8 *drawable = actor->base.drawable;
        u32 zero = 0;
        drawable[7] = 2;
        actor->base.colliderA.responseValue = zero;
        if (!actor->base.drawableKind) {
            u32 *flags = actor->base.drawable;
            u32 clear = ~1;
            *flags &= clear;
        } else {
            u8 *tile = (u8 *)actor->base.drawable + 0x20;
            u32 clear = ~1;
            *(u32 *)(tile + 8) &= clear;
        }
        SceneActorAnimation_SelectDirectionalFlags(actor, 15);
        Sound_PlaySong(475);
    }
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

/* Ivy_UpdateInactiveState: Empty Ivy state handler; inactive role is tentative */

void Ivy_UpdateInactiveState(void)
{
}

/* Ivy_EnterDisabledState: Consumes the entry flag and switches drawable and collision flags to directional animation 20; disabled role tentative */

void Ivy_EnterDisabledState(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        ((u8 *)actor->base.drawable)[7] = 3;
        {
            u16 v = 4 | actor->base.colliderA.flags;
            actor->base.colliderA.flags = v & 0xfff7;
        }
        HalfwordFlags_Set(0x800, &actor->base.placementFlags);
        SceneActorAnimation_SelectDirectionalFlags(&actor->base, 20);
    }
}

struct Player;

s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);

void SceneActorReaction_ApplyHitResponse(void *, u32, struct ContactPosition *, u32, u32, u32);

s32 Ivy_FindAdjacentTarget(struct ChaseActor *, s32);

void Ivy_UpdateHold(struct ChaseActor *actor, s32 frame);

void SceneActorReaction_FinishAnimation17(struct ChaseActor *);

struct IvyOffset {
    u16 x, y, z, pad;
};

extern const struct IvyOffset gUnk_08606384[2][5];

static inline void Ivy_Facing(struct ChaseActor *actor, u8 *quadrant, u8 *flip)
{
    s16 side = ((actor->base.body.direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

static inline void Ivy_EnableCollider(struct ContactCollider *collider)
{
    collider->flags |= 4;
    collider->flags &= 0xFFF7;
}

static inline s32 Ivy_GetElapsed(struct DisplaySequenceState *sequence)
{
    return sequence->elapsed;
}

static inline void Ivy_DisableCollider(struct ContactCollider *collider)
{
    collider->flags &= 0xFFFB;
    collider->flags |= 8;
}

static inline void Ivy_DisableContact(struct ChaseActor *actor)
{
    Ivy_DisableCollider(&actor->base.colliderA);
    SceneActorChase_ClearFlagComponent((void *)&actor->base.placementFlags, 0x800);
}

struct IvyFrameIndex {
    u16 index;
};

static inline struct IvyFrameIndex Ivy_ReadFrame(struct DisplaySequenceState *sequence)
{
    struct IvyFrameIndex value = {0};
    value.index = sequence->index;
    return value;
}

void Ivy_UpdateGrabAttack(struct ChaseActor *actor)
{
    struct DisplaySequenceState *anim;
    struct SceneActorBody *target;
    struct ContactPosition pos;
    u8 quadrant, flip;
    u32 started;

    anim = &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    started = SceneActorReaction_TakePrepared(actor);
    if (started)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if (anim->index == 3 && anim->elapsed == 0)
        Sound_PlaySong(0x1DD);

    if (Ivy_ReadFrame(anim).index == 3 && Ivy_GetElapsed(anim) > 6) {
        actor->base.colliderA.flags |= 4;
        actor->base.colliderA.flags &= 0xFFF7;
        MonsterInit_SetMotionFlags(actor, 0x800);
        if (Ivy_FindAdjacentTarget(actor, 0)) {
            SceneActorChase_SetControl(actor, Ivy_UpdateHold, 0x25);
            SceneActorChase_SetState(actor, 0, 2, 1);

            Ivy_Facing(actor, &quadrant, &flip);
            pos.x = actor->base.body.position.x + gUnk_08606384[flip][anim->index].x;
            pos.y = actor->base.body.position.y + gUnk_08606384[flip][anim->index].y;
            pos.z = actor->base.body.position.z + gUnk_08606384[flip][anim->index].z;
            target = (void *)actor->base.tracking.target;
            if (target->mode <= 3) {
                Player_BeginPositionRequest((void *)gPlayers[target->mode], (void *)&pos, 0, 0,
                                            flip, 0, 0, 0, 0);
            } else if (target->mode == 4) {
                struct ChaseActor *t = target->owner;
                t->base.colliderA.responseValue = 0;
                SceneActorTerrain_ClearCollisionTimers((void *)t);
                SceneActorChase_ClearMotionFlagComponent(t, 0x20);
                SceneActorReaction_ApplyHitResponse(t, 0, (void *)&pos, 0, flip, 0);
            }
            {
                struct Ivy *context = actor->base.context;
                u32 mask = 2;
                context->captureFlags |= mask;
            }
            return;
        }
    } else if (Ivy_ReadFrame(anim).index <= 1) {
        Ivy_DisableContact(actor);
    } else {
        Ivy_EnableCollider(&actor->base.colliderA);
        MonsterInit_SetMotionFlags(actor, 0x800);
    }

    if (actor->base.restart) {
        Ivy_DisableContact(actor);
        SceneActorChase_SetControl(actor, SceneActorReaction_FinishAnimation17, 0x27);
        SceneActorChase_SetState(actor, 0, 2, 3);
    }
}

void Ivy_UpdateActiveState(struct ChaseActor *actor);

void SceneActorReaction_FinishAnimation17(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *destination = &actor->base.colliderA.flags;
        u32 flags = *destination;
        u32 mask = 0xFFFB;
        mask &= flags;
        mask |= 8;
        *destination = mask;
        {
            u16 *p = &actor->base.placementFlags;
            u32 mask = 0xFFFFF7FF;
            *p = mask & *p;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 17);
    }
    if (actor->base.restart) {
        SceneActorChase_SetControl(actor, Ivy_UpdateActiveState, 0x22);
        SceneActorChase_SetBehavior(actor, 2);
    }
}

s32 Div(s32, s32);

s32 Mod(s32, s32);

u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);

void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);

void DamageNumberDisplay_Show(void *, u32, s32, void *, void *, u32);

u32 Ivy_GetAnimationStatus(struct ChaseActor *);

extern s16 *gLinkSession;

struct IvyDrawable {
    u8 unknown0[7];
    u8 mode : 8;
};

static inline void CellCenter(struct SceneVec *out, u32 cell)
{
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    out->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    out->y = (*data & 15) << 8;
    out->z = (z << 8) + 0x80;
}

static inline void SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void ResetState(struct ChaseActor *actor, u32 s)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void SetState(struct ChaseActor *actor, u32 s, u32 p, u32 b)
{
    actor->base.state2A8 = s;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void Facing(struct ChaseActor *actor, u8 *quadrant, u8 *flip)
{
    s32 side = ((actor->base.body.direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

static inline s32 WrapCounter(s32 n)
{
    return n - 256;
}

static inline void FacingAngle(struct ChaseActor *actor, u8 *angle)
{
    s32 turn = (actor->base.body.direction + 0x20) & 0xFF;
    s32 quadrant = turn >= 0 ? turn >> 6 : -(-turn >> 6);
    *angle = quadrant << 6;
}

static inline void ClearHeld(struct Ivy *context)
{
    u32 *flags = &context->captureFlags;
    u32 mask = ~2;
    *flags &= mask;
}

static inline u8 IsIdle(struct Player *player)
{
    if (player->currentStatA == 0)
        return 1;
    return 0;
}

static inline s32 HudMode(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession[0xC];
}

static inline u8 MayCount(struct ChaseActor *victim)
{
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        u32 value = victim->base.hitOwners;
        if (!(value & 0x20) && value) {
            switch (HudMode()) {
                case 0:
                    if (victim->base.hitOwners != 1)
                        return 0;
                    break;
                case 1:
                    if (victim->base.hitOwners != 2)
                        return 0;
                    break;
                case 2:
                    if (victim->base.hitOwners != 4)
                        return 0;
                    break;
                case 3:
                    if (victim->base.hitOwners != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

static inline void ClearVector(void *dest)
{
    CpuFill32(0, dest, 8);
}

static inline void ReportDrain(struct ChaseActor *victim)
{
    struct ContactPackedVector range;
    if (MayCount(victim)) {
        s32 health = victim->base.health;
        u32 max = (u16)victim->base.defaultHealth;
        s32 kind = (s16)victim->base.hudKind;
        s32 parameter = (s16)victim->base.hudLabel;
        SceneHud_SetTimedState(victim->base.body.id, health, max, kind, 0, parameter);
        ClearVector(&range);
        if (victim->base.reactionSoundVariant || !(victim->base.hitAttributes & 0x20000))
            DamageNumberDisplay_Show(&victim->base.colliderA, victim->base.reactionSoundVariant, 0,
                                     &victim->base.body.position, &range, 1);
    }
}

/* Ivy hold (tentative): keeps the held actor or player at the vine's per-frame offset
 * (gUnk_08606384, mirrored for the left-facing quadrants). A held actor (target kind 4) is
 * drained after 140 frames (HUD update, damage number) and handed back with its control
 * callback at group+0x22C; a held player (kind <= 3) is released onto the centre of the
 * actor's cell once its hold time runs out or it stops struggling, otherwise kept in place.
 * Notes for matching: the report is an inline function (its zero temporary lives in the
 * inline's frame, addressed through r8), the drawable mode byte is a bitfield (its store
 * leaves the zero the ROM reuses), and the wrapped tick difference goes through an inline
 * so combine does not refold it. */

void Ivy_UpdateHold(struct ChaseActor *actor, s32 frame)
{
    struct Ivy *context = actor->base.context;
    void *group = (u8 *)actor + 0x3D4;
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    struct SceneActorBody *target;
    u8 quadrant, flip;
    struct ContactPosition position;
    s32 index;
    if (SceneActorReaction_TakePrepared(actor)) {
        context->startTick = (*(u8 **)gPlayers)[0x4F4];
        {
            u16 *destination = &actor->base.colliderA.flags;
            u32 flags = *destination;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *destination = mask;
        }
        {
            u32 mask = 0x800;
            mask |= actor->base.placementFlags;
            actor->base.placementFlags = mask;
        }
    }
    target = (void *)actor->base.tracking.target;
    Facing(actor, &quadrant, &flip);
    if (target->mode <= 3) {
        if (sequence->elapsed + 1 >= sequence->duration) {
            index = sequence->index + 1;
            if (index >= sequence->frameCount)
                index = 0;
        } else
            index = sequence->index;
    } else
        index = sequence->index;
    position.x = actor->base.body.position.x + gUnk_08606384[flip][index].x;
    position.y = actor->base.body.position.y + gUnk_08606384[flip][index].y;
    position.z = actor->base.body.position.z + gUnk_08606384[flip][index].z;
    if (target->mode == 4) {
        struct ChaseActor *victim;
        ((struct IvyDrawable *)actor->base.drawable)->mode = 2;
        victim = target->owner;
        SceneActorReaction_ApplyHitResponse(victim, 0, &position, 0, flip, 0);
        if (frame <= 0x8C)
            goto animate;
        {
            s32 health = victim->base.health;
            victim->base.colliderA.cost = health;
            victim->base.reactionSoundVariant = health;
            victim->base.health = 0;
        }
        ReportDrain(victim);
        ResetState(victim, 3);
        SetCtl(victim, *(void **)((u8 *)group + 0x22C), 0x1E);
        ClearHeld(context);
        actor->base.stepDone = 1;
        return;
    }
    if (target->mode > 3)
        goto animate;
    {
        struct Player *player;
        ((struct IvyDrawable *)actor->base.drawable)->mode = 1;
        player = target->owner;
        if (Mod(frame, context->interval) == 0) {
            s32 start, cur, elapsed;
            if (IsIdle(player))
                goto release;
            start = context->startTick;
            cur = (*(u8 **)gPlayers)[0x4F4];
            if (cur < start) {
                s32 wrapped = WrapCounter(start);
                elapsed = cur - wrapped;
            } else
                elapsed = cur - start;
            if (elapsed > context->holdTicks) {
            release:
                FacingAngle(actor, &quadrant);
                CellCenter((struct SceneVec *)&position, actor->base.terrainCell);
                Player_ApplyPositionRequest(player, (struct SceneVec *)&position, 0x40, quadrant,
                                            0x3C, 0, 0, 0);
                ClearHeld(context);
                SetState(actor, 0, 0, 2);
                SetCtl(actor, (void *)Ivy_UpdateActiveState, 0x22);
                return;
            }
            {
                u32 status = Ivy_GetAnimationStatus(actor);
                u32 request = actor->base.attacks.first[1];
                u32 option = actor->base.attacks.flags[1];
                Player_BeginPositionRequest(player, (struct SceneVec *)&position, 0, status, flip,
                                            0, 0, request, option);
            }
        } else {
            Player_BeginPositionRequest(player, (struct SceneVec *)&position, 0,
                                        Ivy_GetAnimationStatus(actor), flip, 0, 0, 0, 0);
        }
    }
animate:
    SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x12);
    if (sequence->index == 0 && sequence->elapsed == 0)
        Sound_PlaySong(0x1DE);
    if (frame > 0xB4)
        actor->base.stepDone = 1;
}

/* Ivy release start (tentative): on entry disables the main collider, plays directional
 * animation 0x15 and puts a held player back on the centre of the actor's terrain cell; then
 * clears context flag 2. */

void Ivy_BeginRelease(struct ChaseActor *actor)
{
    struct Ivy *context = actor->base.context;
    if (SceneActorReaction_TakePrepared(actor)) {
        {
            u16 *destination = &actor->base.colliderA.flags;
            u32 flags = *destination;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *destination = mask;
        }
        {
            u32 mask = 0x800;
            mask |= actor->base.placementFlags;
            actor->base.placementFlags = mask;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x15);
        {
            struct SceneActorBody *target = (void *)actor->base.tracking.target;
            struct SceneVec position;
            u8 angle;
            CellCenter(&position, actor->base.terrainCell);
            {
                s32 turn = (actor->base.body.direction + 0x20) & 0xFF;
                s32 quadrant = turn >= 0 ? turn >> 6 : -(-turn >> 6);
                angle = quadrant << 6;
            }
            if (target->mode <= 3)
                Player_ApplyPositionRequest(target->owner, &position, 0x40, angle, 0x3C, 0, 0, 0);
        }
        ((u8 *)actor->base.drawable)[7] = 2;
        {
            u32 *flags = &context->captureFlags;
            u32 mask = ~2;
            *flags &= mask;
        }
    }
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

struct IvyTransitionActor {
    u8 unknown0[0x59];
    u8 direction;
    u8 unknown5A[0x8F];
    u8 savedDirection;
    u8 unknownEA[0x1A];
    u32 motion;
    u8 unknown108[0xC];
    u32 flags;
    u8 unknown118[0x22];
    u16 blendFlags;
    u8 unknown13C[0xE0];
    void (*callback)(void);
    u8 unknown220[0x88];
    u8 phase;
    u8 subphase;
    u8 phaseFlags;
    u8 unknown2AB[0x1];
    u32 phaseFrames;
    u8 active;
    u8 restart;
    u8 prepared;
    u8 complete;
    u8 unknown2B4[0x1];
    u8 kind;
    u8 unknown2B6[0x12];
    u16 timer;
    u8 unknown2CA[0x2];
    u32 controlFlags;
    u8 unknown2D0[0x4];
    u16 animationFlags;
};

struct IvyTransitionAnimation {
    u8 unknown0[7];
    u8 mode;
    u16 count;
    u8 unknownA[4];
    u16 frame;
};

static inline void Ivy_SetHalf(u8 *p, u32 offset, u32 mask)
{
    u32 value = mask;
    u16 *field = (void *)(p + offset);
    value |= *field;
    *field = value;
}

static inline void Ivy_ClearWord(u8 *p, u32 offset, u32 bits)
{
    u32 *field = (void *)(p + offset);
    s32 mask = ~bits;
    *field &= mask;
}

static inline void Ivy_SetBlend(u8 *p)
{
    u16 *field = (void *)(p + 0x13A);
    u32 old = *field;
    u32 value = 4;
    value |= old;
    value &= 0xFFF7;
    *field = value;
}

static inline void Ivy_EnterActive(u8 *p, void (*callback)(void), u32 kind)
{
    struct ChaseActor *actor = (void *)p;
    SceneActorChase_SetControl(actor, callback, kind);
    SceneActorChase_SetBehavior(actor, 0);
}

static inline void Ivy_ClearHalf(u8 *p, u32 offset, u32 bits)
{
    struct IvyHalf {
        u16 value;
    };
    struct IvyHalf *field = (void *)(p + offset);
    s32 mask = ~bits;
    field->value &= mask;
}

void SceneActorControl_UpdateAnimationTransition(u8 *p, s32 frame)
{
    struct ChaseActor *actor = (void *)p;
    struct DisplaySequenceState *animation =
        &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    if (SceneActorControl_TakeFlag(&((struct IvyTransitionActor *)p)->prepared)) {
        SceneActorAnimation_SelectDirectionalFlags((struct RoomActor *)p, 3);
        ((struct IvyTransitionActor *)p)->complete = 0;
        Ivy_SetBlend(p);
        Ivy_SetHalf(p, 0x2C0, 0x800);
        Ivy_SetHalf(p, 0x2D4, 0x20);
    }
    Ivy_SetHalf(p, 0x2D6, 0x10);
    if ((((frame <= 17) && (animation->index == 0)) &&
         (animation->elapsed == (animation->duration != 0))) &&
        (animation->duration != 1))
        animation->elapsed = 0;
    if (((struct IvyTransitionActor *)p)->complete) {
        ((struct IvyTransitionActor *)p)->active = 0;
        Ivy_ClearHalf(p, 0x2D4, 0x20);
        Ivy_ClearWord(p, 0x2CC, 0x80);
        Ivy_EnterActive(p, Ivy_UpdateActiveState, 0x22);
        {
            u32 value = ((struct IvyTransitionActor *)p)->savedDirection;
            ((struct IvyTransitionActor *)p)->direction = value;
        }
    }
}

void Ivy_UpdateRestartFlags(u8 *p)
{
    if (SceneActorControl_TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalFlags(p, 0x10);
    if (*(u8 *)(p + 0x2B3)) {
        ActorStatus_SetDrawableFlags((void *)p, 1);
        *(u8 *)(p + 0x2B0) = 1;
    }
}

/* Ivy defeat state (tentative), a variant of Skeleton_UpdateDefeat (Skeleton_UpdateDefeat). On entry:
 * hides the actor, launches a particle burst through sub_08127590 and either plays cue 0xE5
 * (context flag 0x10) or runs the context's defeat script. After 20 frames, without flag 0x10
 * it revives (restores health, SceneActorReaction_PlayHiddenCue); otherwise the full defeat:
 * cue or six-spark burst, terrain and position scripts, event flag, kill counters per
 * category (saturating at 9999, plus one counter per collider reaction flag) and the reward.
 * Kill counters are an if/else (its two stores raise the counter
 * pointer above the reaction flags; cross-jumping merges them). The random table is read through
 * its literal address (REGALLOC.md, "Reload rotation"); the CpuSet source shares a struct
 * with the script values so that it takes the first stack slot. */

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

union HitSparkTable {
    const u16 *table;
    struct {
        u16 offset;
        u8 page;
        u8 region;
    } encoded;
};

static inline const u16 *ReadSparkTable(const union HitSparkTable *r)
{
    return r->table;
}

void sub_08127590(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

void sub_08159164(u32);

void Player_RefreshLifeState(u32, u32);

void SceneActorRecord_RecordPair(u32, u32);

#define STAT(n) (*(s16 *)((u8 *)gGameState + (n)))

#define COUNT(n) Count(&STAT(n))

#define INC(n) STAT(n) = Clamp(STAT(n) + 1)

static inline void Count(s16 *p)
{
    if (*p < 9999)
        *p = *p + 1;
    else
        *p = 9999;
}

static inline s32 Clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}

static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline u32 Masked16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    return *p & mask;
}

static inline u8 TestBits(u32 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}

static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}

static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

#define FLAG_COUNTS()                                                                              \
    {                                                                                              \
        if (flags & 0x80)                                                                          \
            INC(0x58A);                                                                            \
        else if (flags & 0x100)                                                                    \
            INC(0x58C);                                                                            \
        else if (flags & 0x200)                                                                    \
            INC(0x58E);                                                                            \
        else if (flags & 0x400)                                                                    \
            INC(0x590);                                                                            \
        else if (flags &= 0x800)                                                                   \
            INC(0x592);                                                                            \
    }

static inline void RecordKill(struct ChaseActor *actor, u8 *category)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    switch (*category) {
        case 4: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53A);
            FLAG_COUNTS();
            break;
        }
        case 1: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53C);
            FLAG_COUNTS();
            break;
        }
        case 2: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53E);
            FLAG_COUNTS();
            break;
        }
        case 3: {
            u32 flags = collider->reactionFlags;
            COUNT(0x540);
            FLAG_COUNTS();
            break;
        }
        case 14: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54C);
            FLAG_COUNTS();
            break;
        }
        case 24: {
            u32 flags = collider->reactionFlags;
            COUNT(0x55C);
            FLAG_COUNTS();
            break;
        }
        case 10: {
            u32 flags = collider->reactionFlags;
            COUNT(0x546);
            FLAG_COUNTS();
            break;
        }
        case 5:
        case 6:
        case 7: {
            u32 flags = collider->reactionFlags;
            COUNT(0x542);
            FLAG_COUNTS();
            break;
        }
        case 21: {
            u32 flags = collider->reactionFlags;
            COUNT(0x554);
            FLAG_COUNTS();
            break;
        }
        case 18: {
            u32 flags = collider->reactionFlags;
            COUNT(0x550);
            FLAG_COUNTS();
            break;
        }
        case 19: {
            u32 flags = collider->reactionFlags;
            COUNT(0x552);
            FLAG_COUNTS();
            break;
        }
        case 17: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54E);
            FLAG_COUNTS();
            break;
        }
        case 22: {
            u32 flags = collider->reactionFlags;
            COUNT(0x556);
            FLAG_COUNTS();
            break;
        }
        case 20: {
            u32 flags = collider->reactionFlags;
            COUNT(0x558);
            FLAG_COUNTS();
            break;
        }
        case 9: {
            u32 flags = collider->reactionFlags;
            COUNT(0x544);
            FLAG_COUNTS();
            break;
        }
        case 23: {
            u32 flags = collider->reactionFlags;
            COUNT(0x55A);
            FLAG_COUNTS();
            break;
        }
        case 12:
        case 13: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54A);
            FLAG_COUNTS();
            break;
        }
        case 25: {
            u32 flags = collider->reactionFlags;
            COUNT(0x560);
            FLAG_COUNTS();
            break;
        }
    }
}

void SceneActorReaction_PlayHiddenCue(struct ChaseActor *actor);

static inline u8 SaveFlag(u32 maskInput)
{
    u32 mask = maskInput;
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}

static inline void ClearPos(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet((void *)zero, dest, 0x05000002);
}

static inline u16 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(index + RANDOM_TABLE_ADDR);
}

struct DefeatContext {
    u8 unknown0[0x638];
    u16 script;
};

void Ivy_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    struct {
        u32 zero;
        s32 values[8];
    } buf;
    struct Ivy *context = actor->base.context;
    u32 *drawable = actor->base.drawable;
    u8 *categoryp;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct ContactPosition vec;
        s32 single;
        struct ScriptUtilityArguments arguments;
        u32 hidden;
        {
            u16 m = 0x40;
            actor->base.motionFlags = m | actor->base.motionFlags;
        }
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 10, 1);
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            {
                u32 flags = collider->flags;
                flags |= 4;
                flags &= 0xFFF7;
                collider->flags = flags;
            }
        }
        {
            u32 m = 0x800;
            actor->base.placementFlags = m | actor->base.placementFlags;
        }
        SceneActorPlacement_PlayCategoryEventSound(actor);
        ClearPos(&vec, &buf.zero);
        Polar(&vec, actor->base.reactionDirection, Rand() & 0x1F);
        vec.y = (Rand() & 0x1F) + 0x30;
        sub_08127590(&actor->base.body.position, &vec, 0xD638, 0x99DD, 10, 0x1E, 2, 0, 0x12A, 0, 0,
                     0, 0, 0, 0);
        hidden = actor->base.hitReactionFlags & 0x10;
        if (hidden)
            Sound_PlaySong(0xE5);
        else {
            u32 *script = &context->field638;
            if (*script) {
                single = actor->base.body.id;
                arguments.count = 1;
                arguments.values = &single;
                Script_Call(*script, &arguments);
                *script = hidden;
            }
        }
    }
    if (frame <= 0x14)
        return;
    {
        u32 hidden = actor->base.hitReactionFlags & 0x10;
        if (!hidden) {
            s32 h = actor->base.defaultHealth;
            if (h <= 0)
                h = 0;
            actor->base.health = h;
            SceneActorChase_ClearMotionFlagComponent(actor, 0x40);
            SceneActorChase_SetBehavior(actor, 1);
            SceneActorChase_SetControl(actor, SceneActorReaction_PlayHiddenCue, 0x2A);
            {
                struct ContactCollider *collider = &actor->base.colliderA;
                {
                    u32 bit = 4;
                    collider->flags = (collider->flags | 8 | hidden) & ~bit;
                }
            }
            SceneActorChase_ClearFlagComponent((void *)&actor->base.placementFlags, 0x800);
            return;
        }
    }
    if (SaveFlag(0x400)) {
        Sound_PlaySong(0xE5);
        SceneActorChase_SetReactionFlags(actor, 0x1000);
        categoryp = &actor->base.category;
    } else {
        struct ContactPackedVector range;
        const u16 *table;
        union HitSparkTable source = {0};
        u8 category;
        range.x = 80;
        range.y = 60;
        range.z = 60;
        source.encoded.offset = (u16)RANDOM_TABLE_ADDR;
        source.encoded.page = (u8)(RANDOM_TABLE_ADDR >> 16);
        source.encoded.region = (u8)(RANDOM_TABLE_ADDR >> 24);
        table = ReadSparkTable(&source);
        {
            u8 firstCount = SceneActorEffects_NextSparkCount(table);
            SceneActorEffects_SpawnDefeatSparks(actor, (const struct ContactPosition *)&range,
                                                table, firstCount);
        }
        category = actor->base.category;
        categoryp = &actor->base.category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    *drawable |= 1;
    SceneActorBehavior_CallTerrainScript(actor);
    {
        u16 script = (u16)actor->base.script1F8;
        if (script) {
            struct ScriptUtilityArguments arguments;
            buf.values[0] = actor->base.body.position.x;
            buf.values[1] = actor->base.body.position.y;
            buf.values[2] = actor->base.body.position.z;
            buf.values[3] = actor->base.parameters1FC[0];
            buf.values[4] = actor->base.parameters1FC[1];
            buf.values[5] = actor->base.parameters1FC[2];
            buf.values[6] = actor->base.hitOwners;
            buf.values[7] = actor->base.body.id;
            arguments.count = 8;
            arguments.values = buf.values;
            Script_Call(script, &arguments);
        }
    }
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(*categoryp - 11) > 2)
        Bestiary_UnlockEntry(Bestiary_GetEntryForActor(*categoryp, actor->base.paletteOffset));
    SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorChase_SetTypedState(actor, 1, 0, 2);
    COUNT(0x87C);
    COUNT(0x538);
    RecordKill(actor, categoryp);
    sub_08159164(actor->base.experience);
    Player_RefreshLifeState(actor->base.hitAttributes, *((u8 *)actor + 0xA1));
    if (!SceneActorChase_Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->base.body.id);
}

s32 Ivy_SetAuxiliaryColliderFlag(struct ChaseActor *actor)
{
    struct ContactCollider *collider = &actor->base.colliderB;
    collider->flags = 4 | collider->flags;
    return 1;
}

void Collider_SetPosition(void *, void *, u32);

s32 Ivy_UpdateColliderPosition(struct ChaseActor *actor)
{
    Collider_SetPosition(&actor->base.colliderA, &actor->base.body.position, 0);
    return 1;
}

void Ivy_UpdateGrabAttack(struct ChaseActor *actor);

static inline void ClearFlags(u16 *p, u32 mask)
{
    *p = mask & *p;
}

static inline void SetCollider(struct ContactCollider *collider)
{
    u32 flags = ~4;
    flags &= collider->flags;
    flags |= 8;
    collider->flags = flags;
}

void SceneActorControl_WaitForTarget(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct Ivy *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetCollider(&actor->base.colliderA);
        ClearFlags(&actor->base.placementFlags, ~0x800);
    }
    if (!Ivy_FindAdjacentTarget(actor, 1))
        state->targetMissFrames++;
    else
        state->targetMissFrames = 0;
    if ((s32)actor->base.counter2AC > 30 && state->targetMissFrames == 0) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, Ivy_UpdateGrabAttack, 3);
    }
    actor->base.counter2AC++;
}

/* The collider's flags as a 16-bit bitfield: the read-modify-write leaves the zeros that the
 * SetControl and ResetPhase stores reuse. */

struct IvyColliderBits {
    u8 unknown0[6];
    u16 flags : 16;
};

#define SceneActorControl_UpdateDirectionalIdle Ivy_UpdateActiveState

void SceneActorControl_UpdateTargetApproachPhase(struct ChaseActor *actor)
{
    struct Ivy *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *callback = SceneActorReaction_PlayHiddenCue;
        SceneActorChase_SetControl(actor, callback, 0x2A);
        state->targetMissFrames = 0;
    }
    if (actor->base.stepDone) {
        struct IvyColliderBits *collider = (void *)&actor->base.colliderA;
        u32 mask = ~4;
        collider->flags &= mask;
        collider->flags |= 8;
        {
            u16 *flags = &actor->base.placementFlags;
            u32 mask = ~0x800;
            *flags = mask & *flags;
        }
        SceneActorChase_SetControl(actor, Ivy_UpdateActiveState, 0x22);
        SceneActorChase_SetBehavior(actor, 2);
    }
    actor->base.counter2AC++;
}

/* Arm the vine's hold: Ivy_UpdateHold takes over with state 0x25. The
   arguments, the early address and the local constants give the ROM's
   instruction order: agbcc emits code in source evaluation order. */

void Ivy_WaitForHold(struct ChaseActor *actor)
{
    SceneActorControl_TakeStateEntry(actor);
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, Ivy_UpdateHold, 0x25);
        SceneActorChase_SetPhase(actor, 2, 1);
    } else
        actor->base.counter2AC++;
}

void Ivy_ClearAnimationRestartFlag(struct ChaseActor *actor)
{
    if (actor->base.field2B1 != 0)
        actor->base.field2B1 = 0;
}

/* Count frames; after 30, restart the phase (the count still advances).
   The empty test of the taken flag is what the sibling Ivy_WaitLimitThenResetPhase needs
   too: without it cse carries values across and reload picks other
   registers. */

void Ivy_WaitThenResetPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        DEBUG_PRINT();
    }
    if ((s32)actor->base.counter2AC > 30)
        Ivy_ResetPhase((u8 *)actor);
    actor->base.counter2AC++;
}

void Ivy_BeginRelease(struct ChaseActor *actor);

void Ivy_WaitThenBeginRelease(struct ChaseActor *actor)
{
    SceneActorControl_TakeStateEntry(actor);
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, Ivy_BeginRelease, 0x29);
        SceneActorChase_SetPhase(actor, 2, 2);
    }
}

void Ivy_WaitThenResumeActive(struct ChaseActor *actor)
{
    SceneActorControl_TakeStateEntry(actor);
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, Ivy_UpdateActiveState, 0x22);
        SceneActorChase_SetPhase(actor, 0, 2);
    }
}

/* Count frames until the limit at sub + 0x42, then restart the phase.
   Testing the taken flag (even with an empty body) ends cse's extended
   block there, so cse does not carry `sub` across it: the ROM keeps sub in
   a register instead of folding sub + 0x42 into p + 0x416. */

void Ivy_WaitLimitThenResetPhase(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        DEBUG_PRINT();
    }
    if (group->common.gate.duration <= (s32)actor->base.counter2AC)
        Ivy_ResetPhase((u8 *)actor);
    else
        actor->base.counter2AC++;
}

void SceneActorTerrain_DisableCollision(void *);

void SceneActorControl_HideAndDisableCollision(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        ActorStatus_SetDrawableFlags(actor, 1);
        {
            u32 state = 3;
            u8 *field = &actor->base.state2A8;
            u32 zero = 0;
            *field = state;
            actor->base.phase2A9 = zero;
            actor->base.behavior2AA = zero;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
        SceneActorTerrain_DisableCollision(actor);
    }
    actor->base.counter2AC++;
}

void Ivy_UpdateRestartFlags(u8 *p);

void Ivy_UpdateIdle(struct ChaseActor *actor)
{
    if (Chase_TakeArrived(actor))
        SceneActorChase_SetControl(actor, Ivy_UpdateRestartFlags, 0x2E);
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetReactionFlags(actor, 0x1000);
    }
}

extern void *gUnk_086063E4[];

extern void *gUnk_086063D4[];

extern void *gUnk_086063F4[];

void Ivy_InitControlPresets(struct ChaseActor *actor)
{
    actor->base.callbacks284 = gUnk_086063E4;
    actor->base.behaviorCallbacks = gUnk_086063D4;
    actor->base.callbacks294 = gUnk_086063F4;
}

void SceneActorControl_UpdateAnimationTransition(u8 *p, s32 frame);

s32 SceneActorControl_BeginSoundTransition(struct ChaseActor *actor)
{
    u16 *flags;
    u16 suppressed;
    u32 offset;
    u32 value;
    s32 result;
    if (actor->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    {
        u32 mask = 0x100;
        offset = 0x2D4;
        flags = &actor->base.motionFlags;
        value = *flags;
        suppressed = mask & value;
    }
    if (!suppressed) {
        SceneActorChase_SetState(actor, 3, 0, 0);
        SceneActorChase_SetControl(actor, SceneActorControl_UpdateAnimationTransition, 0x2B);
        result = 1;
    } else {
        u32 mask = ~0x20;
        mask &= value;
        *flags = mask;
        result = 0;
    }
    return result;
}

static inline u8 TestFlags_ReleaseCapturedPlayer(struct Ivy *context, u32 maskInput)
{
    u32 mask = maskInput;
    if (context->captureFlags & mask)
        return 1;
    return 0;
}

/* Ivy release (tentative): once per capture (context flag 1), and when context flag 2 says the
 * player is held, puts the player back on the centre of the actor's terrain cell facing the
 * actor's quadrant (cue 0xE6); then returns the actor to idle with both colliders disabled. */

void Ivy_ReleaseCapturedPlayer(struct ChaseActor *actor)
{
    if (TestFlags_ReleaseCapturedPlayer(actor->base.context, 1))
        return;
    if (TestFlags_ReleaseCapturedPlayer(actor->base.context, 2)) {
        struct SceneVec position;
        s32 angle;
        CellCenter(&position, actor->base.terrainCell);
        {
            s32 turn = (actor->base.body.direction + 0x20) & 0xFF;
            angle = (turn >= 0 ? turn >> 6 : -(-turn >> 6)) << 6;
        }
        {
            struct SceneActorBody *target = (void *)actor->base.tracking.target;
            if (target->mode <= 3) {
                Player_ApplyPositionRequest(target->owner, &position, 0x40, angle, 0x3C, 0, 0, 0);
                Sound_PlaySong(0xE6);
            }
        }
        ((u8 *)actor->base.drawable)[7] = 2;
    }
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorChase_SetBehavior(actor, 3);
    {
        struct ContactCollider *collider = &actor->base.colliderA;
        collider->flags |= 4;
    }
    {
        struct ContactCollider *collider = &actor->base.colliderB;
        collider->flags |= 4;
    }
    {
        u32 m = 0x800;
        actor->base.placementFlags = m | actor->base.placementFlags;
    }
    ((struct Ivy *)actor->base.context)->captureFlags |= 1;
    actor->base.health = 0;
}

void SceneActorRegistry_SendToCategory(s32, s16 *);

void Ivy_NotifyGroup(void)
{
    s16 msg = 6;
    SceneActorRegistry_SendToCategory(0x18, &msg);
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Ivy_InitProfileId(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(actor->base.category, 0);
}

struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
    u8 unknown630[4];
    u16 field634;
};

struct IvyPlacement {
    u32 xy, z;
};

/* Ivy drawable reset (tentative): clears the context's capture flags, moves the actor by 0x20
 * on x and z, copies the position to the drawable, selects directional entry 0 and steps the
 * display sequence. */

void Ivy_TickDrawable(struct ChaseActor *actor)
{
    {
        struct IvyContext *context = actor->base.context;
        context->flags = 0;
        context->field634 = 0;
    }
    actor->base.body.position.x += 0x20;
    actor->base.body.position.z += 0x20;
    if (!actor->base.drawableKind)
        *(struct IvyPlacement *)((u8 *)actor->base.drawable + 0x1C) =
            *(struct IvyPlacement *)&actor->base.body.position;
    else
        *(struct IvyPlacement *)((u8 *)actor->base.drawable + 0x40) =
            *(struct IvyPlacement *)&actor->base.body.position;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void Ivy_OnPrimaryContact(void *source, void *target, struct ChaseActor *actor);

void Ivy_OnSecondaryContact(void);

void SceneActorCollision_InitBodyWithFlags(struct ChaseActor *, void *, void *, void (*)(void),
                                           struct ChaseActor *, u32);

void SceneActorCollision_InitAttack(struct ChaseActor *, void *, void *, void (*)(void), u32,
                                    struct ChaseActor *);

void SceneActor_InitCollision5005(struct ChaseActor *actor)
{
    union SceneActorRegistryPackedPosition bodySize, bodyOffset, attackSize, attackOffset;
    bodySize.packed.x = 0x40;
    bodySize.packed.y = 0x7f;
    bodySize.packed.z = 0x40;
    *(u32 *)&bodyOffset = 0x800000;
    *(u32 *)((u8 *)&bodyOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitBodyWithFlags(actor, &bodySize, &bodyOffset, Ivy_OnPrimaryContact,
                                          actor, 0x5005);
    *(u32 *)&attackSize = 0;
    *(u32 *)((u8 *)&attackSize + 4) &= 0xFFFF0000;
    *(u32 *)&attackOffset = 0;
    *(u32 *)((u8 *)&attackOffset + 4) &= 0xFFFF0000;
    SceneActorCollision_InitAttack(actor, &attackSize, &attackOffset, Ivy_OnSecondaryContact, 1,
                                   actor);
}

void SceneActor_BeginReaction22(struct ChaseActor *actor)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = 0;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
    {
        void *callback = Ivy_UpdateActiveState;
        u8 reaction = 0x22;
        actor->base.field2B2 = 1;
        actor->base.stepDone = 0;
        actor->base.controlKind = reaction;
        actor->base.target = callback;
    }
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void *SceneActor_FindNearbyPlayerTarget(struct ChaseActor *actor)
{
    void *target = 0;
    s32 limit = actor->base.tracking.limit * 2;
    struct ContactPosition difference;
    struct Player **slot;
    s32 i;
    u32 flags;
    {
        struct Player **g0 = (struct Player **)0x02000580;
        struct Player *player = g0[0];
        u32 mask = 0x10000;
        flags = player->statusFlags;
        if (mask & flags) {
            limit = 0x24000;
        }
    }
    if ((actor->base.controlKind == 3) && (flags & 0x40000)) {
        return 0;
    }
    {
        u32 blocked = 0x1000;
        struct ContactPosition *delta = &difference;
        const struct ContactPosition *origin = &actor->base.body.position;
        slot = gPlayers;
        i = 0;
        do {
            struct Player *player = *slot;
            if (player && !(player->statusFlags & blocked) && player->positionResetValue == 0) {
                s32 distance;
                const struct SceneVec *position = &player->position;
                delta->x = position->x - origin->x;
                delta->y = position->y - origin->y;
                delta->z = position->z - origin->z;
                distance = delta->x * delta->x + delta->z * delta->z;
                if (limit > distance) {
                    limit = distance;
                    target = &((struct SceneActorChasePlayer *)*slot)->target;
                }
            }
            slot++;
            i--;
        } while (i >= 0);
    }
    return target;
}

/* What a target reference points at: an actor's block at +0x50 (position at +0x5C), or the
 * player record SceneActor_FindNearbyPlayerTarget returns. */

void *SceneActor_FindNearbyPlayerTarget(struct ChaseActor *actor);

struct ChaseActor *SceneActorRegistry_FindNearestCategoryActor(const struct ContactPosition *);

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

/* Row stride of the current terrain map, in cells. */

#define MAP_WIDTH (*(s16 *)(gTerrainMap->cells + 4))

static inline struct SceneActorBody *Keep(struct ChaseActor *actor, struct SceneActorBody *target)
{
    actor->base.tracking.target = (u32)target;
    return target;
}

static inline u32 CellIndex(const struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

/* Ivy target search (tentative): the nearer of SceneActor_FindNearbyPlayerTarget's candidate and the nearest
 * category actor on the same level within the distance limit; it must stand on the terrain
 * cell next to the actor (mode 1: any side, turning to face it; otherwise the side it faces).
 * Stores the result at +0xDC. */

s32 Ivy_FindAdjacentTarget(struct ChaseActor *actor, s32 mode)
{
    s32 best = actor->base.tracking.limit;
    struct ChaseActor *other;
    struct SceneActorBody *target = SceneActor_FindNearbyPlayerTarget(actor);
    struct ContactPosition delta;
    u32 cell;
    if (target && actor->base.body.position.y == target->position.y) {
        s32 distance;
        Delta(&delta, &actor->base.body.position, &target->position);
        distance = delta.x * delta.x + delta.z * delta.z;
        if (best > distance)
            best = distance;
        else
            target = 0;
    }
    other = SceneActorRegistry_FindNearestCategoryActor(&actor->base.body.position);
    if (other && actor->base.body.position.y == other->base.body.position.y) {
        s32 distance;
        Delta(&delta, &actor->base.body.position, &other->base.body.position);
        distance = delta.x * delta.x + delta.z * delta.z;
        if (best > distance)
            target = (void *)((u8 *)other + 0x50);
    }
    if (!target)
        return 0;
    cell = CellIndex(&target->position);
    if (mode == 1) {
        u32 here = actor->base.terrainCell;
        if (here + 1 == cell) {
            actor->base.body.direction = 0;
            return Keep(actor, target);
        }
        if (here - 1 == cell) {
            actor->base.body.direction = 0x80;
            return Keep(actor, target);
        }
        if (here + MAP_WIDTH == cell) {
            actor->base.body.direction = 0x40;
            return Keep(actor, target);
        }
        if (here - MAP_WIDTH == cell) {
            actor->base.body.direction = 0xC0;
            return Keep(actor, target);
        }
        goto fail;
    } else {
        switch (((actor->base.body.direction + 0x20) >> 6) & 3) {
            case 0:
                if (actor->base.terrainCell + 1 != cell)
                    goto fail;
                return Keep(actor, target);
            case 1:
                if (actor->base.terrainCell + MAP_WIDTH != cell)
                    goto fail;
                return Keep(actor, target);
            case 2:
                if (actor->base.terrainCell - 1 != cell)
                    goto fail;
                return Keep(actor, target);
            case 3:
                if (actor->base.terrainCell - MAP_WIDTH != cell)
                    goto fail;
                return Keep(actor, target);
        }
        goto fail;
    }
fail:
    return 0;
}

struct R {
    u8 f[8];
    u16 v;
};

struct PGetAnimationStatus {
    u8 f[4];
    u8 *q;
};

u32 Ivy_GetAnimationStatus(struct ChaseActor *pIn)
{
    struct PGetAnimationStatus *p = (struct PGetAnimationStatus *)pIn;
    struct R *r = (struct R *)(p->q + 0x48);
    if (r->v == 3)
        return 1;
    if (r->v == 4)
        return 0;
    return r->v;
}

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

s32 Ivy_SetAuxiliaryColliderFlag(struct ChaseActor *actor);

s32 Ivy_UpdateColliderPosition(struct ChaseActor *actor);

s32 Ivy_Update(struct ChaseActor *actor)
{
    Ivy_SetAuxiliaryColliderFlag(actor);
    Callback_InvokeActor(actor, actor->base.stateCallbacks[actor->base.state2A8]);
    SceneActorControl_DispatchControl(actor);
    Ivy_UpdateColliderPosition(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(void);

s32 Ivy_UpdateInteraction(void)
{
    SceneActor_UpdateActiveCallbacks();
    return 0;
}

void Sprite_Remove(void *);

void Anim_Remove(void *);

void Heap_Free(void *);

void SceneMessages_UnregisterReceiver(void *);

void Shadow_Destroy(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void Collider_Remove(void *);

void SceneActorRegistry_UnlinkActor(void *);

static inline u8 tst(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

struct SDestroy {
    u8 f0[4];
    u8 *p4;
    u8 b8;
    u8 f1[0x2b7];
    u16 h2c0;
};

u32 Ivy_Destroy(struct SDestroy *s)
{
    u8 *p = s->p4;
    u8 *q;
    u32 m;
    u32 v;
    if (s->b8 == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(p + 0x20);
    Heap_Free(p);
    m = 0x100;
    if (tst((u16 *)((u8 *)s + 0x2c0), m))
        SceneMessages_UnregisterReceiver((u8 *)s + 0xc);
    {
        u8 *r = (u8 *)s + 0x320;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = ((struct ChaseActor *)s)->base.statusFlags & m;
    q = (u8 *)s + 0x50;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove((u8 *)s + 0x134);
    SceneActorRegistry_UnlinkActor(s);
    return 0;
}

void Ivy_InitProfileId(struct ChaseActor *actor);

s32 Ivy_Update(struct ChaseActor *actor);

void Ivy_UpdateDefeat(struct ChaseActor *actor, s32 frame);

s32 SceneActorControl_BeginSoundTransition(struct ChaseActor *actor);

void Ivy_TickDrawable(struct ChaseActor *actor);

void SceneActor_InitCollision5005(struct ChaseActor *actor);

void Ivy_InitControlPresets(struct ChaseActor *actor);

void SceneActor_BeginReaction22(struct ChaseActor *actor);

s32 Ivy_UpdateInteraction(void);

u32 Ivy_Destroy(struct SDestroy *s);

extern const u8 gUnk_08606400[];

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorReaction_SetMotionMode(void);

u32 Route_GetCurrentPointFacing(void *);

s32 TargetRegistry_RegisterActor(void *, void *, void *);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void AnimData_SelectPaletteBank(void *, s32);

s32 Anim_Step(void *, void *);

u8 SceneActorRegistry_LinkActor(void *);

static inline void IvyInit_ResetState(struct ChaseActor *actor, u32 behavior)
{
    MonsterInit_SetState(actor, 0, 0, behavior);
}

static inline void IvyInit_InitializeBody(void *object, u32 *fill)
{
    struct ChaseActor *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit, gateDuration, statValue;
    u16 scriptParameter;
    struct SceneActorRegistryRoute *rt;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = Script_GetKeywordValue(0x4D, 0);
    arg0->base.field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    MonsterInit_InitializeGate(&group->common.gate, gateDuration);
    MonsterInit_ReadRange(&group->common.control);
    MonsterInit_ReadTargeting(&arg0->base.tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    scriptParameter = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->base.script1F0 = scriptParameter;
    scriptParameter = (u16)Script_GetKeywordValue(0x52, 0);
    arg0->base.scriptId = scriptParameter;

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.hudKind = Script_GetKeywordValue(0x4B, 2);
    statValue = (s16)Script_GetKeywordValue(0x56, -1);
    if (statValue < 0)
        statValue = 0xFFFF;
    arg0->base.hudLabel = statValue;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->base.body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor((u8 *)group + 0x30, body, SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, (u8 *)arg0 + 0x40, 0x40, 0x40);

    group->common.bodyId = arg0->base.body.id;
    group->common.bodyPosition = &arg0->base.body.position;
}

static inline void IvyInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Ivy_UpdateInteraction;
    arg0->base.destroy = (SceneActorResultCallback)Ivy_Destroy;
}

static inline void IvyInit_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.routeReset = Ivy_TickDrawable;
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_08606400);
}

static inline void IvyInit_ResetCapture(void *object)
{
    struct ChaseActor *actor = object;
    ((struct Ivy *)actor)->captureFlags = 0;
}

static inline void IvyInit_EnableColliderContact(struct ContactCollider *collider)
{
    u32 flags = collider->flags;
    u32 mask = 8;
    mask |= flags;
    mask &= ~4;
    collider->flags = mask;
}

static inline void IvyInit_ClearMotionFlags(struct ChaseActor *actor, u32 mask)
{
    actor->base.placementFlags = mask & actor->base.placementFlags;
}

s32 Ivy_Init(void *object)
{
    struct ChaseActor *arg0 = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    // concession-ok(const-local): Both CpuSet fills read the same fixed source word.
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    IvyInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_IVY);
    Ivy_InitProfileId(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD638, 0x99DD);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xD638, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    IvyInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct Ivy *)arg0)->field636 = MonsterInit_ReadHalfword(0);
        ((struct Ivy *)arg0)->interval = MonsterInit_ReadByte(0);
        ((struct Ivy *)arg0)->holdTicks = MonsterInit_ReadByte(0);
    }
    ((struct Ivy *)arg0)->field638 = Script_GetKeywordValue('d', 0);
    actor->base.body.position.x += 0x20;
    actor->base.body.position.z += 0x20;
    MonsterInit_DrawablePosition(actor);
    MonsterInit_ReadPalette(actor, 0x12A, 0x192);
    SceneActor_InitCollision5005(actor);
    MonsterInit_InstallOpaqueContactCallbacks(
        actor, Ivy_Update, 0, 0, 0, 0, 0, 0, Ivy_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
        0, SceneActorReaction_ResolvePlacementDamage, SceneActorControl_BeginSoundTransition);
    MonsterInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0, 1);
    IvyInit_InstallRoutes(actor);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x408);
    MonsterInit_SetMotionFlags(actor, 0xC0);
    if (Script_GetKeywordValue('E', 0))
        MonsterInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        IvyInit_ResetState(actor, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Ivy_InitControlPresets(actor);
    SceneActor_BeginReaction22(actor);
    if (MonsterInit_TestWorldFlags(0x400)) {
        IvyInit_ResetState(actor, 1);
        MonsterInit_SetControl(actor, SceneActorReaction_PlayHiddenCue, 0x2A);
        IvyInit_EnableColliderContact(&actor->base.colliderA);
        IvyInit_ClearMotionFlags(actor, ~0x800);
    }
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    IvyInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Ivy_Init(void *object);

void Ivy_Create(void)
{
    u8 *p = Heap_Alloc(0x640);
    if (p != 0) {
        Memory_Clear(p, 0x640);
        if (Ivy_Init(p) < 0) {
            Ivy_Destroy(p);
            Heap_Free(p);
        }
    }
}

s32 SceneActor_RampColorRed(struct PhaseObject *actor);

s32 Ivy_InitColorTransition(struct PhaseObject *p)
{
    u16 *s = &p->color;
    p->green = (*s >> 5) & 0x1f;
    p->blue = (*s >> 10) & 0x1f;
    p->phaseUpdate = (void *)SceneActor_RampColorRed;
}

s32 SceneActor_RampColorRedHigh(struct PhaseObject *);

s32 SceneActor_RampColorRed(struct PhaseObject *actor)
{
    actor->red = Div(actor->pulseTimer << 4, actor->pulseDuration);
    if (actor->pulseTimer++ == actor->pulseDuration) {
        actor->phaseUpdate = (void *)SceneActor_RampColorRedHigh;
        actor->pulseTimer = 0;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}

s32 Ivy_UpdateRedBrightening(struct PhaseObject *p);

s32 SceneActor_RampColorRedHigh(struct PhaseObject *actor)
{
    actor->red = Div(actor->pulseTimer << 3, actor->pulseDuration) + 0x10;
    if (actor->pulseTimer++ == actor->pulseDuration) {
        actor->phaseUpdate = (void *)Ivy_UpdateRedBrightening;
        actor->pulseTimer = 0;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}

static inline u16 color(struct PhaseObject *p)
{
    return ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)p->red;
}

s32 SceneActor_LowerColorRedPeak(struct PhaseObject *actor);

s32 Ivy_WaitForColorRelease(struct PhaseObject *s);

s32 Ivy_UpdateRedBrightening(struct PhaseObject *p)
{
    p->red = Div((u32)p->pulseTimer * 7, p->pulseDuration) + 24;
    if (p->pulseTimer++ == p->pulseDuration) {
        if (p->pulseMode == 0)
            p->phaseUpdate = (void *)SceneActor_LowerColorRedPeak;
        else
            p->phaseUpdate = (void *)Ivy_WaitForColorRelease;
        p->pulseTimer = p->pulseDuration;
    }
    p->color = color(p);
    return 0;
}

s32 ColorPulse_FadeRed(struct PhaseObject *p);

s32 SceneActor_LowerColorRedPeak(struct PhaseObject *actor)
{
    actor->red = Div(actor->pulseTimer * 7, actor->pulseDuration) + 24;
    if (--actor->pulseTimer == -1) {
        actor->phaseUpdate = (void *)ColorPulse_FadeRed;
        actor->pulseTimer = actor->pulseDuration;
    }
    actor->color = (actor->blue << 10) | (actor->green << 5) | actor->red;
    return 0;
}

static inline u16 color_FadeRed(struct PhaseObject *p)
{
    return ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)p->red;
}

s32 ColorPulse_FinishRedFade(struct PhaseObject *p);

s32 ColorPulse_FadeRed(struct PhaseObject *p)
{
    s32 value = Div((u32)p->pulseTimer * 8, p->pulseDuration) + 16;
    s8 *red = &p->red;
    *red = value;
    if (--p->pulseTimer == -1) {
        p->phaseUpdate = (void *)ColorPulse_FinishRedFade;
        p->pulseTimer = p->pulseDuration;
    }
    p->color = ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)*red;
    return 0;
}

static inline u16 color_FinishRedFade(struct PhaseObject *p)
{
    return ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)p->red;
}

s32 Ivy_InitColorTransition(struct PhaseObject *p);

s32 ColorPulse_FinishRedFade(struct PhaseObject *p)
{
    s32 result;
    s32 value = Div((u32)p->pulseTimer * 16, p->pulseDuration);
    s8 *red = &p->red;
    *red = value;
    if (--p->pulseTimer != -1) {
        p->color = ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)*red;
        result = 0;
    } else {
        p->phaseUpdate = (void *)Ivy_InitColorTransition;
        p->pulseTimer = 0;
        p->color = ((u32)p->blue << 10) | ((u32)p->green << 5) | (u32)*red;
        result = 1;
    }
    return result;
}

s32 Ivy_WaitForColorRelease(struct PhaseObject *s)
{
    s32 r;
    if (s->pulseMode != 0)
        r = 1;
    else {
        s->phaseUpdate = (void *)SceneActor_LowerColorRedPeak;
        r = 0;
    }
    return r;
}

/* Split the RGB555 colour at 0xD32 into its 5-bit channels at 0xCC4..0xCC6.
   The original is declared to return a value but has no return statement:
   r0 stays live to the end, so the argument is copied out of it and the
   epilogue pops into r1. */

u32 ColorPulse_UnpackColor(u8 *p)
{
    u16 *color = (u16 *)(p + 0xD32);
    u8 r = *color & 0x1F;
    p[0xCC4] = r;
    r = (*color >> 5) & 0x1F;
    p[0xCC5] = r;
    r = (*color >> 10) & 0x1F;
    p[0xCC6] = r;
}

struct Sub080F2028Position {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};

struct Sub080F2028Scene {
    u8 unknown0[0x32];
    u16 height;
};

struct Sub080F2028Actor {
    u8 unknown0[0x60];
    struct Sub080F2028Scene *scene;
    u8 unknown64[0xB84 - 0x64];
    u16 x;
    u16 unknownB86;
    u16 z;
    u8 unknownB8A[0xC70 - 0xB8A];
    u8 primary[0xD80 - 0xC70];
    s32 frame;
};

extern const s32 gUnk_0860641C[];

// scope shape 2 (differences once low registers are set aside)

// confidence=low: places the primary collider at the actor position offset by a table value.

void Armor_PlacePrimaryCollider(struct Sub080F2028Actor *actor)
{
    struct Sub080F2028Position position;
    u16 *xp;
    const s32 *radius;
    s32 slot;
    const s32 *table;

    slot = Mod(actor->frame, 3);
    xp = &actor->x;
    table = gUnk_0860641C;
    radius = table + slot;
    position.x = *xp + *radius;
    position.y = actor->scene->height + 220;
    position.z = actor->z + *radius;
    Collider_SetPosition(actor->primary, &position, 0);
}

// confidence=low: trigonometric offsets position the main and four paired part colliders.

struct ArmorPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};

struct ArmorScene {
    u8 unknown0[0x32];
    u16 height;
};

struct ArmorSourcePart {
    u8 unknown0[0x1C];
    u16 x, y, z;
    u8 unknown22[0xA];
};

struct ArmorCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4C];
};

struct ArmorOrbitActor {
    u8 unknown0[0x60];
    struct ArmorScene *scene;
    u8 unknown64[0x44];
    struct ArmorSourcePart parts[4];
    u8 unknown158[0x70];
    struct ArmorCollider inner[4], outer[4];
    u8 unknown468[0x4A];
    u8 angle;
    u8 unknown4B3[0x6BB];
    u8 heading;
    u8 unknownB6F[0x15];
    u16 x;
    u16 unknownB86;
    u16 z;
    u8 unknownB8A[0x92];
    struct ArmorCollider primary, secondary;
    u8 unknownCC4[0xBC];
    s32 frame;
};

extern const s16 gUnk_086149C4[];

extern const s32 gUnk_08606428[];

s32 Collider_Refresh(void *);

static inline void Armor_CopyRadius(const s32 **out, const s32 *radius)
{
    *out = radius;
}

void Armor_UpdateOrbitingColliders(struct ArmorOrbitActor *actor)
{
    struct ArmorPosition position;
    const s32 *radius;
    s32 i;
    s32 slot;
    s32 cosine;
    const s32 *table;
    actor->angle = 28 - actor->heading;
    slot = Mod(actor->frame, 3);
    cosine = gUnk_086149C4[(actor->angle + 64) & 255];
    table = gUnk_08606428;
    radius = table + slot;
    position.x = ((cosine * *radius) >> 12) + actor->x;
    position.y = actor->scene->height + 220;
    position.z = ((*radius * gUnk_086149C4[actor->angle]) >> 12) + actor->z;
    Collider_SetPosition(&actor->primary, &position, 0);
    Collider_SetPosition(&actor->secondary, &position, 0);
    if (!(actor->primary.flags & 4))
        Collider_Refresh(&actor->primary);
    for (i = 0; i <= 3; i++) {
        const s32 *loopRadius;
        u8 *angle = &actor->angle;
        const s16 *trig = gUnk_086149C4;
        u16 *point;
        Armor_CopyRadius(&loopRadius, radius);
        point = (void *)&position;
        point[0] = actor->parts[i].x + Div(trig[(*angle + 64) & 255] * *loopRadius, 4096);
        point[1] = actor->scene->height + 220;
        point[2] = actor->parts[i].z + Div(trig[*angle] * *loopRadius, 4096);
        Collider_SetPosition(&actor->outer[i], &position, 0);
        if (!(actor->inner[i].flags & 4)) {
            Collider_Refresh(&actor->inner[i]);
            Collider_SetPosition(&actor->inner[i], &position, 0);
        }
    }
}

/* Armor_UpdatePartColliders_080F2260: same as Armor_UpdatePartColliders but the scene comes from the owner at +0x160 (provisional name) */

struct ArmorVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};

struct ArmorPart {
    u8 pad0[0x1C];
    s16 x, y, z;
    u8 pad22[0x26];
    u8 collider48[6];
    u16 flags4E;
    u8 pad50[0x4C];
    u8 collider9C[0xC4];
    struct {
        u8 pad[0x60];
        struct ArmorSceneUpdatePartColliders_080F2260 *scene;
    } *owner;
};

struct ArmorSceneUpdatePartColliders_080F2260 {
    u8 pad0[0x32];
    s16 height;
};

void Armor_UpdatePartColliders_080F2260(struct ArmorPart *p)
{
    struct ArmorVector position;
    struct ArmorVector delta;
    delta.y = p->owner->scene->height - (s16)(p->y - 220);
    delta.x = delta.z = (delta.y * 724) >> 10;
    position.x = p->x + delta.x;
    position.y = p->y + delta.y;
    position.z = p->z + delta.z;
    Collider_SetPosition(p->collider9C, &position, 0);
    Collider_SetPosition(p->collider48, &position, 0);
    if (!(p->flags4E & 4))
        Collider_Refresh(p->collider48);
}

void Ivy_OnDestroy(void)
{
}

#define ColorPulse_UpdateHitFlash Sword_UpdateBodyPaletteRestore

struct ColorPulseContact {
    u8 pad0[0xa];
    u16 kind;
    u8 padC[0x28];
    u32 source, flags;
    u8 pad3C[2];
    u16 damage;
    u8 pad40[3];
    u8 effect;
    u16 value;
};

struct ColorPulseApplyContact {
    u8 pad0[0x64];
    u16 velocityX;
    u8 pad66[2];
    u16 velocityZ;
    u8 pad6A[0xa];
    u16 impulseX;
    u8 pad76[2];
    u16 impulseZ;
    u8 pad7A[0xa];
    s16 hitValue;
    u8 pad86[0xd];
    u8 mode;
    u8 pad94[8];
    u16 colliderState;
    u8 pad9E[0xb74 - 0x9e];
    void *animation;
    u8 padB78[0xcb4 - 0xb78];
    u16 disabled;
    u8 padCB6[0x12];
    void (*callback)(void);
    u8 padCCC[0x20];
    void (*hitCallback)(void);
    u8 padCF0[0x68];
    u16 palette;
    u8 padD5A[6];
    u32 hitSource, hitFlags, hitKind;
    u8 padD6C[0xc];
    u16 damage, effect;
};

void Ivy_UpdateDefeatBurst(void);

void Sword_UpdateBodyPaletteRestore(void);

void Collision_CalculateDamageResponse(void *, void *);

void PlayerStats_AddContactHundredths(void *, void *, s32);

void ColorPulse_ApplyContact(struct ColorPulseContact *a, struct ColorPulseContact *b,
                             struct ColorPulseApplyContact *p)
{
    if (p->callback == Ivy_UpdateDefeatBurst)
        return;
    if (p->disabled) {
        if (p->effect) {
            b->damage = 0;
            b->effect = 0;
        }
    } else {
        Collision_CalculateDamageResponse(a, b);
        b->value = a->value;
        if (!p->hitCallback) {
            u32 damage = b->damage;
            u32 flags = a->flags;
            if (damage || (flags & 0x40)) {
                u32 source = a->source;
                p->hitSource = source;
                p->hitFlags = flags;
                p->hitKind = a->kind;
                p->colliderState = 8;
                p->hitCallback = Sword_UpdateBodyPaletteRestore;
                AnimData_SelectPaletteBank(p->animation, p->palette);
                PlayerStats_AddContactHundredths(a, b, p->hitValue);
            }
        }
        if (b->damage)
            Sound_PlaySong(318);
        else
            Sound_PlaySong(400);
        {
            u16 *velocity = &p->velocityX;
            if (p->mode == 1) {
                p->impulseX = -*velocity * 4;
                p->impulseZ = -p->velocityZ * 4;
            }
        }
        p->damage = b->damage;
        p->effect = b->effect;
    }
}

void Ivy_InitBobbing(struct PhaseObject *s)
{
    s->offsetX = 0;
    s->unknown66 = 0;
    s->offsetZ = 0;
    s->scatterStepX = 0;
    s->unknown6E = 1;
    s->scatterStepZ = 0;
    s->bobbingTimer = 0x14;
}

void Ivy_UpdateBobbing(struct PhaseObject *s)
{
    u32 *c = &s->bobbingTimer;
    if ((*c & 3) == 0)
        s->unknown66 += s->unknown6E;
    *c = *c - 1;
    if (*c == 0) {
        s->unknown6E = -s->unknown6E;
        *c = 0x28;
    }
    s->position.y += s->unknown66;
}

u32 Terrain_GetHeightAt(void *);

void Ivy_UpdateGroundHeight(struct PhaseObject *s)
{
    if ((s32)s->frame > 7)
        s->landingHeight = (u16)Terrain_GetHeightAt(&s->position);
}

struct IvyAnchorActor;

void Ivy_UpdateAnchorApproach(struct IvyAnchorActor *actor);

void Ivy_WaitForRetarget(struct PhaseObject *p)
{
    if (--p->scatterDelay == 0) {
        p->scatterDelay = 300;
        p->retarget = (void *)Ivy_UpdateAnchorApproach;
    }
}

// confidence=low: approaches the scene anchor, then restores retarget control on expiry.

struct IvyAnchorScene {
    u8 unknown0[0x30];
    u16 x, y, z;
};

struct IvyAnchorActor {
    u8 unknown0[0x60];
    struct IvyAnchorScene *scene;
    u16 motionX, unknown66, motionZ;
    u8 unknown6A[0xA];
    s16 driftX, unknown76, driftZ;
    u8 unknown7A[0x13];
    u8 flags8D, unknown8E[4];
    u8 state, phase, unknown94[0x3F4];
    u16 targetX, targetY, targetZ, unknown48E;
    s16 deltaX, deltaY, deltaZ;
    u8 unknown496[0x1B], direction;
    u8 unknown4B2[0x6D2];
    u16 x, y, z;
    u8 unknownB8A[0x13E];
    void (*memberUpdate)(void);
    u8 unknownCCC[8];
    void (*control)(void);
    u8 unknownCD8[0x1C];
    u32 timer;
};

s32 Math_Atan2(s32, s32);

void ChildGroup_UpdateOrbitingMembers(void);

void Ivy_WaitForRetarget(struct PhaseObject *p);

static inline s32 Ivy_ScaleDirection(s32 value)
{
    if (value >= 0)
        return value >> 10;
    return -((-value) >> 10);
}

static inline s32 Ivy_Halve(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}

static inline void Ivy_SetMode(struct IvyAnchorActor *actor, u8 state, u8 phase)
{
    actor->state = state;
    actor->phase = phase;
    state |= actor->flags8D;
    actor->flags8D = state;
}

void Ivy_UpdateAnchorApproach(struct IvyAnchorActor *actor)
{
    s32 x, z;
    actor->targetX = actor->scene->x;
    actor->targetY = actor->scene->y;
    actor->targetZ = actor->scene->z;
    actor->deltaX = actor->targetX - actor->x;
    actor->deltaY = actor->targetY - actor->y;
    actor->deltaZ = actor->targetZ - actor->z;
    actor->direction = Math_Atan2(actor->deltaX, actor->deltaZ);
    x = gUnk_086149C4[(actor->direction + 64) & 255];
    z = gUnk_086149C4[actor->direction];
    actor->motionX = Ivy_ScaleDirection(x);
    actor->motionZ = Ivy_ScaleDirection(z);
    actor->x += actor->motionX + actor->driftX;
    actor->z += actor->motionZ + actor->driftX;
    actor->driftX = Ivy_Halve(actor->driftX);
    actor->driftZ = Ivy_Halve(actor->driftZ);
    if (--actor->timer == 0) {
        actor->timer = 300;
        Ivy_SetMode(actor, 1, 3);
        actor->memberUpdate = ChildGroup_UpdateOrbitingMembers;
        actor->control = Ivy_WaitForRetarget;
    }
}

void ColorPulse_PrepareTargetDash(void);

void Ivy_WaitForPrimaryActionCompletion(struct PhaseObject *s)
{
    void *update = s->phaseUpdate;
    if (update != 0) {
        if ((u8)Callback_InvokeActor(s, update) != 0)
            s->actionCompletions++;
    }
    if (s->actionCompletions > 1) {
        s->actionCompletions = 0;
        s->transition = (void *)ColorPulse_PrepareTargetDash;
    }
}
