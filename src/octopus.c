#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "display_sequence_inline.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"
#include "octopus.h"
#include "gba/macro.h"
#include "gba/syscall.h"
#include "script_utility.h"
#include "ram.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reset.h"
#include "ivy_phase.h"
#include "debug.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "chase_actor.h"
#include "scene_actor_state.h"
#include "scene_body_api.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "render.h"
#include "engine_globals.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"

void SceneActorBehavior_ApplyContactReaction(void);

void Octopus_OnBodyContact(void)
{
    SceneActorBehavior_ApplyContactReaction();
}

void Octopus_OnAttackContact(void)
{
}

/* Ivy_UpdateActiveState_080E98B4: same as Ivy_UpdateActiveState without setting the owner's byte +7 (provisional name) */

void SceneActorAnimation_SelectDirectionalFlags(struct RoomActor *, s32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u16 *q, u32 m)
{
    *q = m & *q;
}

void Ivy_UpdateActiveState_080E98B4(u8 *p)
{
    if (TakeFlag(p + 0x2B2)) {
        u16 *q;
        q = (u16 *)(p + 0x13A);
        *q = (*q & 0xFFFB) | 8;
        Clr((u16 *)(p + 0x2C0), 0xFFFFF7FF);
    }
    SceneActorAnimation_SelectDirectionalFlags(p, 0xE);
}

void Ivy_UpdateActiveState_080E98B4(u8 *p);

static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}

static inline void clearColliderFlag(u16 *flags)
{
    u32 mask = ~0x800;
    mask &= *flags;
    *flags = mask;
}

static inline void showActor(struct ChaseActor *actor)
{
    if (actor->base.drawableKind == 0)
        clearFlag(actor->base.drawable);
    else {
        u8 *drawable = (u8 *)actor->base.drawable + 0x20;
        u32 mask = ~1;
        *(u32 *)(drawable + 8) &= mask;
    }
}

void SceneActor_BeginReactionAnimation(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u8 *drawable = actor->base.drawable;
        u32 zero = 0;
        drawable[7] = 2;
        actor->base.colliderA.responseValue = zero;
        showActor(actor);
        actor->base.colliderA.flags = (actor->base.colliderA.flags & 0xFFFB) | 8;
        clearColliderFlag(&actor->base.placementFlags);
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 15);
        Sound_PlaySong(0x2A0);
    }
    if (actor->base.restart) {
        SceneActorChase_SetControl(actor, Ivy_UpdateActiveState_080E98B4, 0x22);
    }
}

/* Octopus_UpdateIdleState: Empty hook within the Octopus behavior code with idle role tentative */


void Octopus_UpdateIdleState(void)
{
}

struct DamageTarget {
    u8 pad[8];
    u8 kind;
    u8 rest[0x33];
    void *owner;
};

void SpriteParticle_SpawnBatch(void *, s32, s32, s32, s32);

void SpriteEffect_SpawnTowardObject(void *, void *, u32, u32, u32, u32);

static inline void SetControl(struct ChaseActor *actor, u32 kind, void *target)
{
    void *callback = target;
    u32 type = kind;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = type;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline void HideCollider(struct ChaseActor *actor)
{
    struct ContactCollider *collider = &actor->base.colliderA;
    u32 f = collider->flags;
    u32 m = 4;
    m |= f;
    m &= 0xFFF7;
    collider->flags = m;
    {
        u32 m = 0x800;
        actor->base.placementFlags = m | actor->base.placementFlags;
    }
}

static inline void ShowCollider(struct ChaseActor *actor)
{
    actor->base.colliderA.flags = (actor->base.colliderA.flags & 0xFFFB) | 8;
    {
        u32 m = ~0x800;
        actor->base.placementFlags = m & actor->base.placementFlags;
    }
}

// confidence=low: applies a contact effect to the held player and waits before release.

void Ivy_ApplyHeldTargetDamage(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        struct DamageTarget *target = (void *)actor->base.tracking.target;
        if (target->kind == 4) {
            SetControl(actor, 16, SceneActorControl_OnIdle);
            SetState(actor, 0, 0, 1);
            return;
        }
        if (target->kind <= 3) {
            u8 *player = target->owner;
            u32 a3, a4, a5, a6;
            SpriteParticle_SpawnBatch(&actor->base.body.position, 1, 2, 0, 0x154);
            a3 = actor->base.attacks.first[0];
            a4 = actor->base.attacks.second[0];
            a5 = actor->base.attacks.flags[0];
            a6 = actor->base.attacks.parameter[0];
            SpriteEffect_SpawnTowardObject((u8 *)actor + 0x50, player + 0x24, a3, a4, a5, a6);
            Sound_PlaySong(0x1E0);
        }
        HideCollider(actor);
    }
    if (frame > 60) {
        SetControl(actor, 16, SceneActorControl_OnIdle);
        SetState(actor, 0, 0, 4);
    }
}

void SceneActor_EndReactionAnimation(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *destination = &actor->base.colliderA.flags;
        u32 flags = *destination;
        u32 mask = 4;
        mask |= flags;
        mask &= 0xFFF7;
        *destination = mask;
        {
            u32 mask = 0x800;
            mask |= actor->base.placementFlags;
            actor->base.placementFlags = mask;
        }
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 16);
    }
    if (actor->base.restart) {
        actor->base.colliderA.responseValue = 0;
        actor->base.stepDone = 1;
        if (!actor->base.drawableKind)
            *(u32 *)actor->base.drawable |= 1;
        else {
            struct SceneInputWidget *tile = (void *)((u8 *)actor->base.drawable + 0x20);
            tile->flags |= 1;
        }
    }
}

struct OctopusCaptureTarget {
    u8 unk0[8];
    u8 kind;
    u8 unk9[0x3C - 9];
    struct ChaseActor *actor;
};

struct Player;

s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);

void SceneActorReaction_ApplyHitResponse(void *, u32, struct ContactPosition *, u32, u32, u32);

struct IvyTargetRef;

struct IvyTargetRef *SceneActorReaction_FindAdjacentTarget(struct ChaseActor *, s32);

void Octopus_UpdateHold(struct ChaseActor *actor, s32 frame);

void SceneActorReaction_PlayEntry17(struct ChaseActor *);

struct OctopusOffset {
    u16 x, y, z, pad;
};

extern const struct OctopusOffset gUnk_086062B0[2][5];

static inline void Octopus_GetCaptureFacing(struct ChaseActor *actor, u8 *quadrant, u8 *flip)
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

static inline void Octopus_EnableCollider(struct ContactCollider *collider)
{
    collider->flags |= 4;
    collider->flags &= 0xFFF7;
}

static inline void Octopus_SetContactFlag(struct ChaseActor *actor, u32 bits)
{
    actor->base.placementFlags = bits | actor->base.placementFlags;
}

static inline void Octopus_DisableContact(struct ChaseActor *actor)
{
    actor->base.colliderA.flags &= 0xFFFB;
    actor->base.colliderA.flags |= 8;
    SceneActorChase_ClearFlagComponent((void *)&actor->base.placementFlags, 0x800);
}

struct OctopusFrameIndex {
    u16 index;
};

static inline struct OctopusFrameIndex Octopus_ReadFrame(struct DisplaySequenceState *sequence)
{
    struct OctopusFrameIndex value = {0};
    value.index = sequence->index;
    return value;
}

void Octopus_UpdateCaptureAttack(struct ChaseActor *actor)
{
    struct DisplaySequenceState *anim;
    struct OctopusCaptureTarget *target;
    struct ContactPosition pos;
    u8 quadrant, flip;
    u32 started;
    s32 limit;
    u16 *palette;

    anim = &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    started = SceneActorReaction_TakePrepared(actor);
    palette = &actor->base.paletteOffset;
    if (started) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        if (*palette == 1)
            Anim_SetDurationScale(anim, 0x30);
    }
    if (anim->index == 3 && anim->elapsed == 0)
        Sound_PlaySong(0x1DD);

    if (*palette == 1)
        limit = 3;
    else
        limit = 6;

    if (Octopus_ReadFrame(anim).index == 3 && anim->elapsed > limit) {
        Octopus_EnableCollider(&actor->base.colliderA);
        Octopus_SetContactFlag(actor, 0x800);
        if (SceneActorReaction_FindAdjacentTarget(actor, 0)) {
            SceneActorChase_SetControl(actor, Octopus_UpdateHold, 0x25);
            SceneActorChase_SetState(actor, 0, 2, 1);
            SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x12);

            Octopus_GetCaptureFacing(actor, &quadrant, &flip);
            pos.x = actor->base.body.position.x + gUnk_086062B0[flip][anim->index].x;
            pos.y = actor->base.body.position.y + gUnk_086062B0[flip][anim->index].y;
            pos.z = actor->base.body.position.z + gUnk_086062B0[flip][anim->index].z;
            target = (void *)actor->base.tracking.target;
            if (target->kind <= 3) {
                Player_BeginPositionRequest(gPlayers[target->kind], (void *)&pos,
                                            0, 0, flip, 0, 0, 0, 0);
            } else if (target->kind == 4) {
                struct ChaseActor *t = target->actor;
                t->base.colliderA.responseValue = 0;
                SceneActorTerrain_ClearCollisionTimers((void *)t);
                SceneActorChase_ClearMotionFlagComponent(t, 0x20);
                SceneActorReaction_ApplyHitResponse(t, 0, (void *)&pos, 0, flip, 0);
            }
            return;
        }
    } else if (Octopus_ReadFrame(anim).index <= 1) {
        Octopus_DisableContact(actor);
    } else {
        actor->base.colliderA.flags |= 4;
        actor->base.colliderA.flags &= 0xFFF7;
        Octopus_SetContactFlag(actor, 0x800);
    }

    if (actor->base.restart) {
        Octopus_DisableContact(actor);
        SceneActorChase_SetControl(actor, SceneActorReaction_PlayEntry17, 0x27);
        SceneActorChase_SetState(actor, 0, 2, 3);
    }
}

void SceneActorReaction_PlayEntry17(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.colliderA.flags = (actor->base.colliderA.flags & 0xFFFB) | 8;
        {
            u16 *field = &actor->base.placementFlags;
            u32 mask = ~0x800;
            *field = mask & *field;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 17);
    }
    {
        u32 controlOffset = 0x2B3;
        if (*((u8 *)actor + controlOffset)) {
            void *callback = Ivy_UpdateActiveState_080E98B4;
            u8 kind = 34;
            u8 *prepared = &actor->base.field2B2;
            u32 zero = 0;
            u8 one = 1;
            u32 offset;
            u32 mask;
            u8 byteZero;
            *prepared = one;
            controlOffset -= 3;
            *(((u8 *)actor) - (-controlOffset)) = zero;
            controlOffset += 5;
            *((u8 *)actor + controlOffset) = kind;
            actor->base.target = callback;
            offset = 0x2C8;
            byteZero = 0;
            *(u16 *)((u8 *)actor + offset) = zero;
            {
                u32 *field = &actor->base.pendingControlFlags;
                mask = ~1;
                *field = *field & mask;
            }
            actor->base.pendingControl = zero;
            {
                u32 state = 3;
                actor->base.state2A8 = byteZero;
                actor->base.phase2A9 = byteZero;
                {
                    u32 behOff = 0x2AA;
                    u8 *bq;
                    bq = (u8 *)actor + behOff;
                    *bq = state;
                }
                actor->base.counter2AC = zero;
                actor->base.field2B1 = one;
            }
        }
    }
}

s32 Div(s32, s32);

s32 Mod(s32, s32);

u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);

void SceneHud_SetTimedState(u32, s32, u32, u32, u32, u32);

void DamageNumberDisplay_Show(void *, u32, s32, void *, void *, u32);

u32 Octopus_GetAnimationStatus(struct ChaseActor *);

void Octopus_UpdateActiveState(void);

extern s16 *gLinkSession;

struct OctopusTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    void *player;
};

struct OctopusDrawable {
    u8 unknown0[7];
    u8 mode : 8;
};

static inline u8 TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

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

static inline void ClearHeld(struct Octopus *context)
{
    u32 *flags = &context->flags628;
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
    if (TestFlags(gEngineFlags, 0x4800)) {
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

// MATCHING_CONCESSION: register-pinning
void Octopus_UpdateHold(struct ChaseActor *actor, s32 frame)
{
    struct Octopus *context = actor->base.context;
    void *group = (u8 *)actor + 0x3D4;
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    struct OctopusTarget *target;
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
    if (target->kind <= 3) {
        if (sequence->elapsed + 1 >= sequence->duration) {
            index = sequence->index + 1;
            if (index >= sequence->frameCount)
                index = 0;
        } else
            index = sequence->index;
    } else
        index = sequence->index;
    position.x = actor->base.body.position.x + gUnk_086062B0[flip][index].x;
    position.y = actor->base.body.position.y + gUnk_086062B0[flip][index].y;
    position.z = actor->base.body.position.z + gUnk_086062B0[flip][index].z;
    if (target->kind == 4) {
        struct ChaseActor *victim;
        ((struct OctopusDrawable *)actor->base.drawable)->mode = 2;
        victim = target->player;
        SceneActorReaction_ApplyHitResponse(victim, 0, &position, 0, flip, 0);
        goto animate;
    }
    if (target->kind > 3)
        goto animate;
    {
        struct Player *player;
        ((struct OctopusDrawable *)actor->base.drawable)->mode = 1;
        player = target->player;
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
                SetState(actor, 0, 0, 0);
                SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
                return;
            }
            {
                u32 status = Octopus_GetAnimationStatus(actor);
                u32 request = actor->base.attacks.first[1];
                u32 option = actor->base.attacks.flags[1];
                Player_BeginPositionRequest(player, (struct SceneVec *)&position, 0, status, flip,
                                            0, 0, request, option);
            }
        } else {
            Player_BeginPositionRequest(player, (struct SceneVec *)&position, 0,
                                        Octopus_GetAnimationStatus(actor), flip, 0, 0, 0, 0);
        }
    }
animate:
    SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x12);
    if (sequence->index == 0 && sequence->elapsed == 0)
        Sound_PlaySong(0x1DE);
    if (frame > 0xB4) {
        actor->base.stepDone = 1;
        ((struct OctopusDrawable *)actor->base.drawable)->mode = 1;
    }
}

// confidence=low: release animation for a captured target; on the release frame, throws a

// captured actor (kind & 4) or player (kind <= 3) from the arm's offset.

static inline void Hide(struct ChaseActor *a)
{
    u32 bit = 1;
    if (a->base.drawableKind == 0)
        *(u32 *)a->base.drawable |= bit;
    else {
        u8 *p = (u8 *)a->base.drawable + 0x20;
        *(u32 *)(p + 8) |= bit;
    }
}

extern const struct OctopusOffset gUnk_08606300[2][2];

void *SceneActorRegistry_FindById(u32);

void Octopus_ThrowCapturedTarget(struct ChaseActor *actor)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    struct OctopusTarget *target;
    s32 index;
    u8 quadrant, flip;
    struct ContactPosition position;
    void *who;
    void *player;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x13);
        {
            u16 *d = &actor->base.colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->base.placementFlags = bit | actor->base.placementFlags;
        }
    }
    target = (void *)actor->base.tracking.target;
    if (target->kind <= 3 && sequence->elapsed + 1 >= sequence->duration) {
        index = sequence->index + 1;
        if (index >= sequence->frameCount)
            index = 0;
    } else
        index = sequence->index;
    Facing(actor, &quadrant, &flip);
    position.x = actor->base.body.position.x + gUnk_08606300[flip][index].x;
    position.y = actor->base.body.position.y + gUnk_08606300[flip][index].y;
    position.z = actor->base.body.position.z + gUnk_08606300[flip][index].z;
    if (target->kind & 4) {
        who = target->player;
        SceneActorReaction_ApplyHitResponse(who, 1, &position, 0, flip, 0);
    } else if (target->kind <= 3) {
        player = target->player;
        Player_BeginPositionRequest(player, (void *)&position, 0, 3, flip, 0, 0, 0, 0);
    }
    if (actor->base.restart) {
        actor->base.stepDone = 1;
        ((struct OctopusDrawable *)actor->base.drawable)->mode = 2;
        Hide(actor);
        Sound_PlaySong(0x1E0);
        if (target->kind & 4) {
            who = SceneActorRegistry_FindById(*(u32 *)target);
            SceneActorReaction_ApplyHitResponse(who, 2, &position, 0, 0, 0);
            SpriteParticle_SpawnBatch(&actor->base.body.position, 1, 2, 0, 0x154);
        } else if (target->kind <= 3) {
            player = target->player;
            Player_BeginPositionRequest(player, (void *)&position, 0, 3, flip, 0, 1, 0, 0);
            SpriteParticle_SpawnBatch(&actor->base.body.position, 1, 1, 0, 0x154);
        }
    }
}

// confidence=low: slam that ends a capture; at frames 60/90 the arm reappears and the held

// player (kind <= 3) or actor (kind & 4) is released, after frame 120 it returns to idle.

struct GolemRoutePoint {
    s16 x, z;
};

struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};

static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

void SceneActorBehavior_PlayReactionSound(void *);

void Camera_RequestShake(u32);

void SceneActorReaction_RestoreAlternateAfterDelay(void);

static inline void Show(struct ChaseActor *a)
{
    if (a->base.drawableKind == 0) {
        u32 *d = a->base.drawable;
        u32 mask = ~1;
        *d &= mask;
    } else {
        u8 *p = (u8 *)a->base.drawable + 0x20;
        u32 mask = ~1;
        *(u32 *)(p + 8) &= mask;
    }
}

static inline void Palette(struct ChaseActor *a, u32 bank)
{
    if (a->base.drawableKind == 0)
        AnimData_SelectPaletteBank((u8 *)a->base.drawable + 0x2C, bank);
    else
        *(u8 **)((u8 *)a->base.drawable + 0x68) = gPaletteBankData + bank * 32;
}

void Octopus_ReleaseCapturedTarget(struct ChaseActor *actor, s32 frame)
{
    u8 *group;
    u8 *drawable = actor->base.drawable;
    struct OctopusTarget *target;
    struct ContactPosition position;
    struct Player *player;
    void *victim;
    group = (u8 *)actor + 0x3D4;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry(actor, 0x14);
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            u32 flags = collider->flags;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            collider->flags = mask;
        }
        {
            u32 bit = 0x800;
            actor->base.placementFlags = bit | actor->base.placementFlags;
        }
        {
            struct ContactPosition *dest = &actor->base.body.position;
            struct GolemRouteLink *link = (void *)(group + 0x74);
            dest->x = link->point->x;
            dest->z = link->point->z;
            dest->y = CellHeight(dest, CellIndex(dest));
        }
        *(struct ContactPosition *)(drawable + 0x1C) = actor->base.body.position;
    }
    target = (void *)actor->base.tracking.target;
    if (frame == 60 || frame == 90) {
        Show(actor);
        Palette(actor, actor->base.paletteAlternateBank);
        {
            u32 eight = 8;
            actor->base.flashTimer = eight;
        }
        {
            u32 bit = 0x8000;
            actor->base.behaviorFlags |= bit;
        }
        if (target->kind <= 3) {
            u32 request, option;
            player = target->player;
            request = actor->base.attacks.first[2];
            option = actor->base.attacks.flags[2];
            Player_BeginPositionRequest(player, (void *)&actor->base.body.position, 0, 0, 0, 0, 1,
                                        request, option);
            Sound_PlaySong(0xE6);
        } else if (target->kind & 4) {
            victim = target->player;
            SceneActorReaction_ApplyHitResponse(victim, 2, &position, 0, 0, 0);
            SceneActorBehavior_PlayReactionSound(victim);
        }
    } else
        Hide(actor);
    if ((u32)(frame - 51) <= 8)
        Camera_RequestShake(20);
    if ((u32)(frame - 81) <= 8)
        Camera_RequestShake(40);
    if (frame > 120) {
        Hide(actor);
        if (target->kind & 4) {
            victim = target->player;
            SceneActorReaction_ApplyHitResponse(victim, 2, &position, 0, 0x40, 0);
        } else if (target->kind <= 3) {
            s32 turn, angle;
            player = target->player;
            turn = (actor->base.body.direction + 0x20) & 0xFF;
            angle = (turn >= 0 ? turn >> 6 : -(-turn >> 6)) << 6;
            CellCenter((void *)&position, actor->base.terrainCell);
            Player_ApplyPositionRequest(player, (void *)&position, 0x80, angle, 60, 0, 0, 0);
            SpriteParticle_SpawnBatch(&actor->base.body.position, 1, 1, 1, 0x154);
            Sound_PlaySong(0x1DF);
        }
        SceneActorAnimation_SelectDirectionalFlags(actor, 0x10);
        {
            struct Delay {
                u8 pad[8];
                u8 a, b, c, d;
                u32 ticks;
                void *cb;
            };
            struct Delay *delay = (void *)(group + 0x8C);
            void *cb = SceneActorControl_OnIdle;
            u32 one = 1;
            delay->ticks = 60;
            delay->cb = cb;
            delay->a = 0;
            delay->b = 0;
            delay->c = one;
        }
        SetCtl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 14);
        SetState(actor, 3, 0, 0);
        actor->base.body.position.x += 32;
        actor->base.body.position.z += 32;
        *(struct ContactPosition *)(drawable + 0x1C) = actor->base.body.position;
    }
}

// Remaining differences: register allocation and instruction scheduling; see notes/codex-low.md.

void SceneActorTerrain_DisableCollision(void *);

void Octopus_ReturnToRoutePoint(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u8 *group = (u8 *)actor + 0x3D4;
        u16 *bank;
        struct ContactPosition *dest;
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            u32 flags = collider->flags;
            u32 bit = 4;
            bit |= flags;
            bit &= 0xFFF7;
            collider->flags = bit;
        }
        {
            u32 bit = 0x800;
            actor->base.placementFlags = bit | actor->base.placementFlags;
        }
        SceneActorAnimation_SelectProfileEntry(actor, 0x14);
        {
            u16 *b = (void *)((u8 *)actor + 0x1E0);
            u32 v = *b + 3;
            u32 kind = actor->base.drawableKind;
            bank = b;
            if (kind == 0)
                AnimData_SelectPaletteBank((u8 *)actor->base.drawable + 0x2C, v);
            else
                *(u8 **)((u8 *)actor->base.drawable + 0x68) = gPaletteBankData + v * 32;
        }
        Hide(actor);
        ((struct OctopusDrawable *)actor->base.drawable)->mode = 3;
        SpriteParticle_SpawnBatch(&actor->base.body.position, 1, 2, 1, 0x154);
        Sound_PlaySong(0x1D9);
        SceneActorTerrain_DisableCollision(actor);
        {
            u16 *offset = &actor->base.paletteOffset;
            u32 value = *bank + *offset;
            Palette(actor, value);
        }
        SetCtl(actor, SceneActorControl_OnIdle, 16);
        SetState(actor, 3, 0, 0);
        dest = &actor->base.body.position;
        {
            struct GolemRouteLink *link = (void *)(group + 0x74);
            dest->x = link->point->x;
            dest->z = link->point->z;
            dest->y = CellHeight(dest, CellIndex(dest));
        }
        if (actor->base.drawableKind == 0)
            *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x1C) =
                actor->base.body.position;
        else
            *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x40) =
                actor->base.body.position;
    }
}

// confidence=low: stagger reaction; once it finishes, hands control to

// SceneActor_EndReactionAnimation (kind 0x23) and restarts behaviour 3.

void SceneActor_EndReactionAnimation(struct ChaseActor *actor);

void Octopus_UpdateStagger(struct ChaseActor *actor, s32 frame)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->base.drawable + 0x48);
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        actor->base.restart = 0;
        {
            u16 *d = &actor->base.colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->base.placementFlags = bit | actor->base.placementFlags;
        }
        {
            u32 flags = 0x20;
            flags |= actor->base.motionFlags;
            actor->base.motionFlags = flags;
        }
    }
    {
        u32 flags = 0x10;
        flags |= actor->base.reactionFlags;
        actor->base.reactionFlags = flags;
    }
    if (frame <= 17 && sequence->index == 0) {
        u16 elapsed = sequence->elapsed;
        u8 duration = sequence->duration;
        if (elapsed == (duration != 0) && duration != 1)
            sequence->elapsed = 0;
    }
    if (actor->base.restart) {
        actor->base.stepDone = 0;
        {
            u16 bit = 0x20;
            actor->base.motionFlags &= ~bit;
        }
        {
            u32 *field = &actor->base.behaviorFlags;
            u32 mask = ~0x80;
            *field &= mask;
        }
        SetCtl(actor, SceneActor_EndReactionAnimation, 0x23);
        SetState(actor, 1, 0, 3);
        actor->base.body.direction = actor->base.tracking.heading;
    }
}

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorRegistry_SendToActorId(u8, void *, u16);

extern const s16 gUnk_086149C4[];

static inline u16 Octopus_RandomSample(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}

static inline u16 Octopus_RandomSpeed(void)
{
    return (Octopus_RandomSample(gRandomTable) & 7) + 0x48;
}

static inline u32 Octopus_SpinSpeed(u32 value)
{
    return value | ~0x1F;
}

inline static u32 Octopus_MotionBits(u32 mask, u16 *flags)
{
    return (*flags) & mask;
}

struct OctopusDefeatMessage {
    u16 kind, value;
};

static inline void Octopus_MessageValue(void *object, u16 value)
{
    struct OctopusDefeatMessage *message = object;
    message->value = value;
}

static inline void Octopus_EnableDefeatMotion(struct ChaseActor *actor, u16 bit, u16 **component)
{
    u16 *field = &actor->base.motionFlags;
    *component = field;
    *field = bit | *field;
}

void Octopus_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    u8 *context = actor->base.context;
    u8 *drawable = actor->base.drawable;
    s32 values[8];
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 3);
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            collider->flags |= 4;
            collider->flags &= 0xFFF7;
        }
        {
            u32 bit = 0x800;
            actor->base.placementFlags = bit | actor->base.placementFlags;
        }
        {
            u16 bit = 0x40;
            actor->base.motionFlags &= ~bit;
        }
        {
            u8 k1 = 1;
            actor->base.health = k1;
        }
        actor->base.heightOffset = 0;
        Memory_Clear(context + 0x624, 8);
        {
            s32 abs;
            actor->base.secondaryMotion.y = Octopus_RandomSpeed();
            *((s16 *)(((u8 *)context) + 0x62A)) =
                Octopus_SpinSpeed(Octopus_RandomSample(gRandomTable));
            actor->base.field2E8 = 0;
            abs = *((s16 *)(((u8 *)context) + 0x62A));
            if (abs < 0)
                abs = -abs;
            if (abs < 10)
                *((s16 *)(((u8 *)context) + 0x62A)) = 10;
        }
        {
            u32 bit = 2;
            if (actor->base.drawableKind == 0)
                *((u32 *)actor->base.drawable) |= bit;
            else {
                u8 *p = ((u8 *)actor->base.drawable) + 0x20;
                *((u32 *)(p + 8)) |= bit;
            }
        }
        SceneActorPlacement_PlayCategoryEventSound(actor);
    }
    drawable[6] += context[0x62A];
    {
        u32 angle = (0x1A0 - drawable[6]) & 0xFF;
        *((u16 *)(((u8 *)drawable) + 0x20)) -= (gUnk_086149C4[angle & 0xFF] * 33) >> 11;
        *((u16 *)(((u8 *)drawable) + 0x1C)) -= (gUnk_086149C4[(angle + 0x40) & 0xFF] * 33) >> 11;
    }
    {
        u8 k2 = 0x10;
        if (actor->base.statusFlags & k2) {
            u16 *flags;
            struct ContactPosition *pos;
            Octopus_EnableDefeatMotion(actor, 0x40, &flags);
            actor->base.health = 0;
            pos = &actor->base.body.position;
            SpriteParticle_SpawnBatch(pos, 1, 2, 0, 0x154);
            Sound_PlaySong(0x1D8);
            {
                u8 *d;
                u32 bit = 1;
                if (actor->base.drawableKind == 0) {
                    u32 *sequence = actor->base.drawable;
                    *sequence |= bit;
                    d = (void *)sequence;
                } else {
                    u8 *p;
                    d = actor->base.drawable;
                    p = d + 0x20;
                    *((u32 *)(p + 8)) |= bit;
                }
                if (actor->base.drawableKind == 0) {
                    u32 mask = ~2;
                    *((u32 *)d) &= mask;
                } else {
                    u32 mask;
                    d += 0x20;
                    mask = ~2;
                    *((u32 *)(d + 8)) &= mask;
                }
            }
            SceneActorBehavior_CallTerrainScript(actor);
            {
                u16 script = *((u16 *)(((u8 *)actor) + 0x1F8));
                if (script) {
                    struct ScriptUtilityArguments arguments;
                    values[0] = pos->x;
                    values[1] = actor->base.body.position.y;
                    values[2] = actor->base.body.position.z;
                    values[3] = *((u32 *)(((u8 *)actor) + 0x1FC));
                    values[4] = *((u32 *)(((u8 *)actor) + 0x200));
                    values[5] = *((u32 *)(((u8 *)actor) + 0x204));
                    values[6] = actor->base.hitOwners;
                    values[7] = actor->base.body.id;
                    arguments.count = 8;
                    arguments.values = values;
                    Script_Call(script, &arguments);
                }
            }
            if ((!Octopus_MotionBits(0x4000, flags)) && (((u32)(actor->base.category - 11)) > 2))
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
            SceneActorChase_SetTypedControl(actor, (void *)SceneActorControl_OnIdle, 0x10);
            SceneActorChase_SetTypedState(actor, 1, 0, 2);
            {
                u8 *linked = actor->base.context;
                struct OctopusDefeatMessage *message = (void *)values;
                u32 id;
                message->kind = 8;
                Octopus_MessageValue(message, 0);
                id = *((u32 *)(((u8 *)linked) + 0x630));
                if (id)
                    SceneActorRegistry_SendToActorId(15, values, id);
            }
        } else if (Mod(frame, 4) == 0)
            actor->base.secondaryMotion.y -= 8;
    }
}

struct T {
    u8 f[6];
    u16 flags;
};

struct S {
    u8 filler[0x188];
    struct T t;
};

s32 Octopus_SetAnimationRefreshFlag(struct S *s)
{
    struct T *t = &s->t;
    u32 m = 4;
    t->flags = m | t->flags;
    return 1;
}

void Collider_SetPosition(void *, void *, u32);

s32 Octopus_UpdateBodyColliderPosition(u8 *s)
{
    Collider_SetPosition(s + 0x134, s + 0x5c, 0);
    return 1;
}

void SceneActor_BeginReactionAnimation(struct ChaseActor *actor);

static inline void SetControl_WaitHiddenForAdjacentTarget(struct ChaseActor *a, void *cb, u32 k)
{
    a->base.field2B2 = 1;
    a->base.stepDone = 0;
    a->base.controlKind = k;
    a->base.target = cb;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.pendingControlFlags;
        u32 mask = ~1;
        *p &= mask;
    }
    a->base.pendingControl = 0;
}

static inline void State(struct ChaseActor *a, u8 state, u8 phase, u8 behavior)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

static inline void Or(u16 m, u16 *f)
{
    *f |= m;
}

// confidence=low: hides the actor and waits for an adjacent target; one starts the reaction

// animation, otherwise after 60 frames it moves on to phase 2, behavior 5.


void SceneActorReaction_WaitHiddenForAdjacentTarget(struct ChaseActor *a)
{
    if (SceneActorControl_TakeFlag(&a->base.field2B1)) {
        u32 flag;
        {
            struct ContactCollider *c = &a->base.colliderA;
            u16 v = 4 | c->flags;
            c->flags = v & 0xfff7;
        }
        Or(0x800, &a->base.placementFlags);
        flag = 1;
        if (a->base.drawableKind == 0) {
            u32 *flags = a->base.drawable;
            *flags |= flag;
        } else {
            u32 *flags = (u32 *)((u8 *)a->base.drawable + 0x20);
            flags[2] |= flag;
        }
    }
    if (SceneActorReaction_FindAdjacentTarget(a, 1) == 0) {
        if ((s32)a->base.counter2AC > 60)
            State(a, 0, 2, 5);
        else
            a->base.counter2AC++;
    } else {
        State(a, 0, 0, 2);
        SetControl_WaitHiddenForAdjacentTarget(a, (void *)SceneActor_BeginReactionAnimation, 41);
    }
}

void SceneActorControl_OnIdle(u8 *);

void Octopus_UpdateCaptureAttack(struct ChaseActor *actor);

static inline void SetControl_ReacquireAdjacentTarget(struct ChaseActor *a, void *cb, u32 k)
{
    a->base.field2B2 = 1;
    a->base.stepDone = 0;
    a->base.controlKind = k;
    a->base.target = cb;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.pendingControlFlags;
        u32 mask = ~1;
        *p &= mask;
    }
    a->base.pendingControl = 0;
}

static inline void State_ReacquireAdjacentTarget(struct ChaseActor *a, u32 state)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = 0;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

static inline void IdleState(struct ChaseActor *a)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = 0;
    a->base.behavior2AA = 0;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

// confidence=low: reacquires an adjacent target after the entry animation, or times out to idle.

void SceneActorReaction_ReacquireAdjacentTarget(struct ChaseActor *a)
{
    if (SceneActorControl_TakeFlag(&a->base.field2B1)) {
        SetControl_ReacquireAdjacentTarget(a, (void *)SceneActor_BeginReactionAnimation, 41);
        a->base.restart = 0;
    }
    if (a->base.restart || a->base.controlKind == 34) {
        if (SceneActorReaction_FindAdjacentTarget(a, 1)) {
            if (!a->base.drawableKind) {
                u32 *p = a->base.drawable;
                u32 mask = ~1;
                *p &= mask;
            } else {
                u32 *p = (void *)((u8 *)a->base.drawable + 0x20);
                u32 mask = ~1;
                p[2] &= mask;
            }
            State_ReacquireAdjacentTarget(a, 3);
            SetControl_ReacquireAdjacentTarget(a, (void *)Octopus_UpdateCaptureAttack, 3);
        }
        if (a->base.controlKind == 34 && (s32)a->base.counter2AC > 180) {
            IdleState(a);
            SetControl_ReacquireAdjacentTarget(a, (void *)SceneActorControl_OnIdle, 16);
            return;
        }
    }
    a->base.counter2AC++;
}

void *SceneActorTarget_FindNearbyPlayer(struct ChaseActor *actor);

void Ivy_ApplyHeldTargetDamage(struct ChaseActor *actor, s32 frame);

static inline u8 TakeReset(struct ChaseActor *actor)
{
    u8 *flag = &actor->base.field2B1;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline void ResetState_AwaitTargetControl(struct ChaseActor *actor, u32 state, u32 phase,
                                                 u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}

void SceneActorReaction_AwaitTargetControl(struct ChaseActor *actor)
{
    void *target;
    TakeReset(actor);
    target = SceneActorTarget_FindNearbyPlayer(actor);
    if (target) {
        void *callback;
        u8 kind;
        u8 *prepared;
        u32 zero;
        u8 byteZero;
        u32 state;
        actor->base.tracking.target = (u32)target;
        callback = Ivy_ApplyHeldTargetDamage;
        kind = 45;
        prepared = &actor->base.field2B2;
        zero = 0;
        *prepared = 1;
        actor->base.stepDone = zero;
        actor->base.controlKind = kind;
        actor->base.target = callback;
        {
            u16 *field = &actor->base.field2C8;
            byteZero = 0;
            *field = zero;
        }
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
        ResetState_AwaitTargetControl(actor, 3, 0, 0);
    } else {
        if ((s32)actor->base.counter2AC > 300 || SceneActorReaction_FindAdjacentTarget(actor, 1)) {
            u32 zero = (u32)target;
            void *callback;
            u32 kind;
            ResetState_AwaitTargetControl(actor, 0, 0, 1);
            callback = SceneActorControl_OnIdle;
            kind = 16;
            actor->base.field2B2 = 1;
            actor->base.stepDone = zero;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = zero;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = zero;
        }
        actor->base.counter2AC++;
    }
}

struct AdjacentWaitContext {
    u8 pad[0x634];
    s16 missed, limit;
};

struct AdjacentWaitTarget {
    u8 pad[0xC];
    struct ContactPosition position;
};

s32 Math_Atan2(s32, s32);

static inline void BeginControl(struct ChaseActor *actor, s32 zero)
{
    u32 three = 3;
    u32 stateOffset = 0x2A8;
    u32 phaseOffset = 0x2A9;
    u8 *field = (u8 *)actor + stateOffset;
    *field = three;
    field = (u8 *)actor + phaseOffset;
    *field = zero;
    stateOffset += 2;
    field = (u8 *)actor + stateOffset;
    *field = zero;
    phaseOffset += 3;
    *(u32 *)((u8 *)actor + phaseOffset) = zero;
    stateOffset += 7;
    field = (u8 *)actor + stateOffset;
    *field = 1;
    {
        void *callback = Octopus_UpdateCaptureAttack;
        stateOffset += 1;
        *((u8 *)actor + stateOffset) = 1;
        phaseOffset = 0x2B0;
        *((u8 *)actor + phaseOffset) = zero;
        stateOffset += 3;
        *((u8 *)actor + stateOffset) = three;
        phaseOffset -= 0x94;
        *(void **)((u8 *)actor + phaseOffset) = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->base.pendingControl = zero;
    }
}

void SceneActorReaction_WaitForAdjacentTarget(struct ChaseActor *a)
{
    struct AdjacentWaitContext *ctx = a->base.context;
    struct AdjacentWaitTarget *target = (void *)a->base.tracking.target;
    s32 found;
    s32 zero;
    if (SceneActorControl_TakeFlag(&a->base.field2B1))
        ctx->missed = 0;
    found = SceneActorReaction_FindAdjacentTarget(a, 1);
    if (!found) {
        if (ctx->missed > ctx->limit)
            ResetState(a, found);
        else
            ctx->missed++;
    } else
        ctx->missed = 0;
    if ((s32)a->base.counter2AC > 30 && (zero = ctx->missed) == 0) {
        struct ContactPosition *position = &a->base.body.position;
        a->base.body.direction =
            Math_Atan2(target->position.x - position->x, target->position.z - position->z);
        BeginControl(a, zero);
    }
    a->base.counter2AC++;
}

static inline void SetControl_RestoreAfterControl(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorReaction_RestoreAfterControl(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SetControl_RestoreAfterControl(actor, SceneActor_EndReactionAnimation, 35);
        actor->base.stepDone = 0;
    }
    if (actor->base.stepDone) {
        struct SceneActorReactionCommand *command = (void *)&actor->group.saved.control;
        void *callback = SceneActorControl_OnIdle;
        u32 one = 1;
        u32 state;
        command->duration = 30;
        command->payload = callback;
        command->first = 0;
        command->second = 0;
        command->kind = one;
        state = 3;
        actor->base.state2A8 = state;
        actor->base.phase2A9 = 0;
        actor->base.behavior2AA = 0;
        actor->base.counter2AC = 0;
        {
            u16 offset = 0x2B1;
            *((u8 *)actor + offset) = one;
        }
        SetControl_RestoreAfterControl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 14);
    }
}

static inline void SetControl_StartHoldControl(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetStateB(struct ChaseActor *actor)
{
    u32 two = 2;
    u8 *field = &actor->base.state2A8;
    *field = 0;
    actor->base.phase2A9 = two;
    actor->base.behavior2AA = 1;
    actor->base.counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}

void Octopus_StartHoldControl(struct ChaseActor *actor)
{
    TakeReset(actor);
    if (actor->base.stepDone) {
        SetControl_StartHoldControl(actor, Octopus_UpdateHold, 0x25);
        SetStateB(actor);
    } else
        actor->base.counter2AC++;
}

struct SClearStateChangedFlag {
    u8 filler[0x000002B1];
    u8 f;
};

void Octopus_ClearStateChangedFlag(struct SClearStateChangedFlag *s)
{
    if (s->f != 0)
        s->f = 0;
}

/* Count frames; after 60, restart the phase (the count still advances).
   The empty test of the taken flag is what the sibling Ivy_WaitLimitThenResetPhase needs
   too: without it cse carries values across and reload picks other
   registers. */

void Octopus_WaitThenResetPhase(u8 *p)
{
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
        DEBUG_PRINT();
    }
    if (*(s32 *)(p + 0x2AC) > 60)
        Ivy_ResetPhase(p);
    (*(s32 *)(p + 0x2AC))++;
}

void Octopus_ThrowCapturedTarget(struct ChaseActor *actor);

static inline void SetControl_EnterThrowPhase(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetState_EnterThrowPhase(struct ChaseActor *actor)
{
    u32 two = 2;
    u32 behavior = 2;
    u8 *field = &actor->base.state2A8;
    *field = 0;
    actor->base.phase2A9 = two;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}

void Octopus_EnterThrowPhase(struct ChaseActor *actor)
{
    TakeReset(actor);
    if (actor->base.stepDone) {
        SetControl_EnterThrowPhase(actor, Octopus_ThrowCapturedTarget, 0x26);
        SetState_EnterThrowPhase(actor);
    }
}

void Octopus_ReleaseCapturedTarget(struct ChaseActor *actor, s32 frame);

static inline void SetControl_EnterReleasePhase(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void SetState_EnterReleasePhase(struct ChaseActor *actor)
{
    u32 two = 2;
    u32 behavior = 4;
    u8 *field = &actor->base.state2A8;
    *field = 0;
    actor->base.phase2A9 = two;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = 1;
    }
}

void Octopus_EnterReleasePhase(struct ChaseActor *actor)
{
    TakeReset(actor);
    if (actor->base.stepDone) {
        SetControl_EnterReleasePhase(actor, Octopus_ReleaseCapturedTarget, 0x28);
        SetState_EnterReleasePhase(actor);
    }
}

struct SClearAnimationRestartFlag {
    u8 filler[0x000002B1];
    u8 f;
};

void Octopus_ClearAnimationRestartFlag(struct SClearAnimationRestartFlag *s)
{
    if (s->f != 0)
        s->f = 0;
}

/* Like Ivy_ResetPhase, but the phase's behaviour byte restarts at 1. */

static inline void Ivy_ResetPhaseActive(u8 *p)
{
    u32 one = 1;
    u8 *state = &p[0x2A8];
    u32 zero = 0;
    *state = zero;
    p[0x2A9] = zero;
    p[0x2AA] = one;
    *(u32 *)(p + 0x2AC) = zero;
    p[0x2B1] = one;
}

/* Count frames until the limit at sub + 0x42, then restart the phase
   (see Ivy_WaitLimitThenResetPhase for the empty test of the taken flag). */

void Octopus_WaitLimitThenResetPhase(u8 *p)
{
    u8 *sub = p + 0x3D4;
    if (SceneActorControl_TakeFlag(&p[0x2B1])) {
        DEBUG_PRINT();
    }
    if (*(u16 *)(sub + 0x42) <= *(s32 *)(p + 0x2AC))
        Ivy_ResetPhaseActive(p);
    else
        (*(s32 *)(p + 0x2AC))++;
}

static inline void SetControl_ReturnToIdle(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

static inline void ResetState_ReturnToIdle(struct ChaseActor *actor)
{
    u32 one = 1;
    u8 *field = &actor->base.state2A8;
    u32 zero = 0;
    *field = one;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = zero;
    actor->base.counter2AC = zero;
    {
        u16 offset = 0x2A9;
        offset += 8;
        *((u8 *)actor + offset) = one;
    }
}

void SceneActorReaction_ReturnToIdle(struct ChaseActor *actor)
{
    TakeReset(actor);
    if (actor->base.stepDone) {
        ResetState_ReturnToIdle(actor);
        SetControl_ReturnToIdle(actor, SceneActorControl_OnIdle, 16);
    }
}

struct RoomActor;

void SceneActorReaction_BeginHiddenCollisionless(struct ChaseActor *actor)
{
    if (TakeReset(actor)) {
        u32 mask = 1;
        if (!actor->base.drawableKind)
            *(u32 *)actor->base.drawable |= mask;
        else {
            struct SceneInputWidget *tile = (void *)((u8 *)actor->base.drawable + 0x20);
            tile->flags |= mask;
        }
        ResetState(actor, 3);
        SceneActorTerrain_DisableCollision((void *)actor);
    }
    actor->base.counter2AC++;
}

extern const u8 gUnk_08606338[];

extern const u8 gUnk_08606320[];

extern const u8 gUnk_0860634C[];

struct P {
    const u8 *a;
    u32 pad;
};

struct SInitControlPresets {
    u8 filler[0x284];
    struct P p[3];
};

void Octopus_InitControlPresets(struct SInitControlPresets *s)
{
    s->p[0].a = gUnk_08606338;
    s->p[1].a = gUnk_08606320;
    s->p[2].a = gUnk_0860634C;
}

/* SceneActorControl_BeginSoundTransition_080EBC38: SceneActorControl_BeginSoundTransition with other constants (ported by relatives.py + repair.py; provisional name) */

void Octopus_UpdateStagger(struct ChaseActor *, s32);

static inline void BeginTransition(struct ChaseActor *actor, u16 zero, u32 offset)
{
    u32 state = 3;
    u32 stateOffset = 0x2A8;
    u8 *field = (u8 *)actor + stateOffset;
    *field = state;
    offset -= 0x2B;
    *((u8 *)actor + offset) = zero;
    actor->base.behavior2AA = zero;
    stateOffset += 4;
    *(u32 *)((u8 *)actor + stateOffset) = zero;
    offset += 8;
    *((u8 *)actor + offset) = 1;
    {
        void *callback = Octopus_UpdateStagger;
        u32 kind = 0x2A;
        offset++;
        *((u8 *)actor + offset) = 1;
        stateOffset = 0x2B0;
        *((u8 *)actor + stateOffset) = zero;
        offset += 3;
        *((u8 *)actor + offset) = kind;
        stateOffset -= 0x94;
        *(void **)((u8 *)actor + stateOffset) = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
    }
}

s32 SceneActorControl_BeginSoundTransition_080EBC38(struct ChaseActor *actor)
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
        flags = (void *)((u8 *)actor + offset);
        value = *flags;
        suppressed = mask & value;
    }
    if (!suppressed) {
        BeginTransition(actor, suppressed, offset);
        result = 1;
    } else {
        u32 mask = ~0x20;
        mask &= value;
        *flags = mask;
        result = 0;
    }
    return result;
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

struct SInitProfileId {
    u8 filler[0x98];
    u8 b;
    u8 f2[0x308 - 0x99];
    s32 v;
};

void Octopus_InitProfileId(struct SInitProfileId *s)
{
    s->v = SceneActorProfiles_FindOrAdd(s->b, 0);
}

/* SceneActorControl_UpdateMotionPhase_080EBD0C: same code as SceneActorControl_UpdateMotionPhase at another address (ported by relatives.py; provisional name) */

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
 * display sequence (the step is ScenePanelRig_TickSprites's, with the same pins). The drawable
 * pointer passes through r0, as in the ROM. */

void SceneActorControl_UpdateMotionPhase_080EBD0C(struct ChaseActor *actor)
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

/* Tentative name based on the observed state transition or initialization. */

union SceneActorCollisionVector {
    struct ContactPackedVector vector;
    u32 words[2];
};

void SceneActorCollision_InitBodyWithFlags(struct ChaseActor *, void *, void *, void (*)(void),
                                           void *, u32);

void SceneActorCollision_InitAttack(struct ChaseActor *, void *, void *, void (*)(void), u32,
                                    void *);

void Octopus_OnBodyContact(void);

void Octopus_OnAttackContact(void);

void SceneActorCollision_InitCompactBody(struct ChaseActor *actor)
{
    union SceneActorCollisionVector bounds, offset, attackBounds, attackOffset;
    bounds.vector.x = 64;
    bounds.vector.y = 256;
    bounds.vector.z = 64;
    offset.words[0] = 0x800000;
    offset.vector.z = 0;
    SceneActorCollision_InitBodyWithFlags(actor, &bounds, &offset, Octopus_OnBodyContact, actor,
                                          0x5005);
    attackBounds.words[0] = 0;
    attackBounds.vector.z = 0;
    attackOffset.words[0] = 0;
    attackOffset.vector.z = 0;
    SceneActorCollision_InitAttack(actor, &attackBounds, &attackOffset, Octopus_OnAttackContact, 1,
                                   actor);
}

void Octopus_ReturnToRoutePoint(struct ChaseActor *actor);

void SceneActorTerrain_ResumeDrawable(struct ChaseActor *);

static inline void SetState_BeginReturnToRoutePoint(struct ChaseActor *actor, u32 s)
{
    u8 *state = &actor->base.state2A8;
    u32 zero = 0;
    *state = s;
    actor->base.phase2A9 = zero;
    actor->base.behavior2AA = zero;
    actor->base.counter2AC = zero;
    actor->base.field2B1 = 1;
}

struct Delay {
    u8 pad[8];
    u8 a, b, c, d;
    u32 ticks;
    void *cb;
};

/* Octopus: settle into state 3, return to the route point after 60 ticks,
   restore the alternate pose after 14, and raise the drawable flag. */

void Octopus_BeginReturnToRoutePoint(struct ChaseActor *actor)
{
    SetState_BeginReturnToRoutePoint(actor, 3);
    {
        struct Delay *delay = (void *)((u8 *)actor + 0x460);
        void *cb = Octopus_ReturnToRoutePoint;
        delay->ticks = 60;
        delay->cb = cb;
        delay->a = 3;
        delay->b = 0;
        delay->c = 0;
    }
    SetCtl(actor, SceneActorReaction_RestoreAlternateAfterDelay, 14);
    SceneActorTerrain_ResumeDrawable(actor);
    {
        u8 *a = (u8 *)actor;
        if (a[8] == 0)
            **(u32 **)(a + 4) |= 1;
        else {
            u32 *d = *(u32 **)(a + 4) + 8;
            d[2] |= 1;
        }
    }
}

static inline void SetControl_BeginIdleAnimation(struct ChaseActor *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void Octopus_BeginIdleAnimation(struct ChaseActor *actor)
{
    u32 flag;
    SceneActorAnimation_SelectDirectionalFlags(actor, 0x10);
    flag = 1;
    if (actor->base.drawableKind == 0) {
        u32 *flags = actor->base.drawable;
        *flags |= flag;
    } else {
        u32 *flags = (u32 *)((u8 *)actor->base.drawable + 0x20);
        flags[2] |= flag;
    }
    {
        u32 one = 1;
        actor->base.state2A8 = 0;
        actor->base.phase2A9 = 0;
        actor->base.behavior2AA = one;
        actor->base.counter2AC = 0;
        actor->base.field2B1 = one;
    }
    SetControl_BeginIdleAnimation(actor, SceneActorControl_OnIdle, 0x10);
}

/* The target handle starts twelve bytes before the player's position. */

struct ScenePlayerTargetView {
    u8 unknown0[0x24];
    struct {
        u8 unknown0[0xC];
        struct ContactPosition position;
    } target;
};

union ScenePlayerSlot {
    struct Player *player;
    struct ScenePlayerTargetView *tracking;
};

extern u8 *gUnknown_02000580[];

static inline void SubtractPosition(s16 *delta, const struct SceneVec *a,
                                    const struct ContactPosition *b)
{
    delta[0] = a->x - b->x;
    delta[1] = a->y - b->y;
    delta[2] = a->z - b->z;
}

void *SceneActorTarget_FindNearbyPlayer(struct ChaseActor *actor)
{
    void *result = 0;
    s32 limit = actor->base.tracking.limit * 2;

    u32 flags;
    s16 delta[4];
    union ScenePlayerSlot *players;
    struct ContactPosition *position;
    s32 count;
    u32 excluded;
    s16 *difference;
    {
        struct Player *initial = (struct Player *)gUnknown_02000580[0];
        u32 mask = 0x10000;
        flags = initial->statusFlags;
        if (flags & mask) {
            limit = 0x24000;
        }
    }
    if ((actor->base.controlKind == 3) && (flags & 0x40000)) {
        return 0;
    }
    excluded = 0x1000;
    difference = delta;
    position = &actor->base.body.position;
    players = (union ScenePlayerSlot *)gPlayers;
    count = 0;
    do {
        struct Player *player = players->player;
        if (player && !(player->statusFlags & excluded)) {
            s32 distance;
            SubtractPosition(difference, &player->position, position);
            distance = difference[0] * difference[0] + difference[2] * difference[2];
            if (limit > distance) {
                limit = distance;
                result = &players->tracking->target;
            }
        }
        players++;
        count--;
    } while (count >= 0);
    return result;
}

/* What a target reference points at: an actor's block at +0x50 (position at +0x5C), or the
 * player record SceneActorTarget_FindNearbyPlayer returns. */

struct IvyTargetRef {
    u8 unknown0[0xC];
    struct ContactPosition position;
};

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

static inline struct IvyTargetRef *Keep(struct ChaseActor *actor, struct IvyTargetRef *target)
{
    actor->base.tracking.target = (u32)target;
    return target;
}

/* Ivy target search (tentative): the nearer of SceneActorTarget_FindNearbyPlayer's candidate and the nearest
 * category actor on the same level within the distance limit; it must stand on the terrain
 * cell next to the actor (mode 1: any side, turning to face it; otherwise the side it faces).
 * Stores the result at +0xDC. */

struct IvyTargetRef *SceneActorReaction_FindAdjacentTarget(struct ChaseActor *actor, s32 mode)
{
    s32 best = actor->base.tracking.limit;
    struct ChaseActor *other;
    struct IvyTargetRef *target = SceneActorTarget_FindNearbyPlayer(actor);
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

struct A {
    u8 f[8];
    u16 v;
};

u32 Octopus_GetAnimationStatus(struct ChaseActor *actor)
{
    struct A *a = (struct A *)((u8 *)actor->base.drawable + 0x48);
    if (a->v == 3)
        return 1;
    if (a->v == 4)
        return 0;
    return a->v;
}

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

s32 Octopus_SetAnimationRefreshFlag(struct S *s);

s32 Octopus_UpdateBodyColliderPosition(u8 *s);

struct SUpdate {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
};

s32 Octopus_Update(struct SUpdate *s)
{
    Octopus_SetAnimationRefreshFlag(s);
    Callback_InvokeActor(s, s->tbl[s->idx]);
    SceneActorControl_DispatchControl(s);
    Octopus_UpdateBodyColliderPosition(s);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(void);

s32 Octopus_UpdateInteraction(void)
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

u32 Octopus_Destroy(struct SDestroy *s)
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

void Octopus_InitProfileId(struct SInitProfileId *s);

s32 Octopus_Update(struct SUpdate *s);

void Octopus_UpdateDefeat(struct ChaseActor *actor, s32 frame);

s32 SceneActorControl_BeginSoundTransition_080EBC38(struct ChaseActor *actor);

void SceneActorControl_UpdateMotionPhase_080EBD0C(struct ChaseActor *actor);

void SceneActorCollision_InitCompactBody(struct ChaseActor *actor);

void Octopus_InitControlPresets(struct SInitControlPresets *s);

void Octopus_BeginIdleAnimation(struct ChaseActor *actor);

s32 Octopus_UpdateInteraction(void);

u32 Octopus_Destroy(struct SDestroy *s);

extern const u8 gUnk_0860635C[];

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

u32 SceneActorReaction_OnEnemyDefault(void);

u8 SceneActorTarget_UpdateTrackedContact(void *);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorReaction_SetMotionMode(void);

void SceneActorMovement_ResolveRoutePosition(void *);

void SceneActorBehavior_InitializeShadow(void *);

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

static inline void OctopusInit_DrawablePosition(struct ChaseActor *actor)
{
    if (actor->base.drawableKind == 0)
        *(struct ContactPosition *)(actor->base.drawable + 0x1C) = actor->base.body.position;
    else
        *(struct ContactPosition *)(actor->base.drawable + 0x40) = actor->base.body.position;
}

static inline void OctopusInit_InitializeBody(void *object, u32 *fill)
{
    struct ChaseActor *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, variant;
    u16 scriptParameter;
    struct SceneActorRegistryRoute *rt;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = Script_GetKeywordValue('M', 0);
    arg0->base.field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue('N', 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword('l')) {
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
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    scriptParameter = (u16)Script_GetKeywordValue('F', 0);
    arg0->base.script1F0 = scriptParameter;
    scriptParameter = (u16)Script_GetKeywordValue('R', 0);
    arg0->base.scriptId = scriptParameter;

    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword('X')) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.hudKind = Script_GetKeywordValue('K', 2);
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    arg0->base.hudLabel = variant;
    if (Script_GetKeywordValue('H', 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue('L', 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->base.body.position, rt);
        OctopusInit_DrawablePosition(arg0);
        arg0->base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor(&group->common.placement, body,
                                     SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x40, 0x40);

    group->common.bodyId = arg0->base.body.id;
    group->common.bodyPosition = &arg0->base.body.position;
}

static inline void OctopusInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Octopus_UpdateInteraction;
    arg0->base.destroy = (SceneActorResultCallback)Octopus_Destroy;
}

static inline void OctopusInit_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.routeReset = SceneActorControl_UpdateMotionPhase_080EBD0C;
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_0860635C);
}

static inline void OctopusInit_ResetCapture(void *object)
{
    struct ChaseActor *actor = object;
    ((struct Octopus *)actor)->captureFlags = 0;
}

s32 Octopus_Init(void *object)
{
    struct ChaseActor *arg0 = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    OctopusInit_ResetCapture(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_OCTOPUS);
    Octopus_InitProfileId(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD291, 0xDCEB);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0xD291, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    OctopusInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        ((struct Octopus *)arg0)->field636 = MonsterInit_ReadHalfword(0);
        ((struct Octopus *)arg0)->interval = MonsterInit_ReadByte(0);
        ((struct Octopus *)arg0)->holdTicks = MonsterInit_ReadByte(0);
    }
    actor->base.body.position.x += 0x20;
    actor->base.body.position.z += 0x20;
    OctopusInit_DrawablePosition(actor);
    MonsterInit_ReadPalette(actor, 0x248, 0x192);
    SceneActorCollision_InitCompactBody(actor);
    MonsterInit_InstallSequenceCallbacks(actor, Octopus_Update, 0, 0, 0, 0, 0, 0,
                                         Octopus_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                                         0, SceneActorReaction_ResolvePlacementDamage,
                                         SceneActorControl_BeginSoundTransition_080EBC38);
    MonsterInit_InitializeGroup(group, 0, 0, 0, 0, 0, 0, 1);
    OctopusInit_InstallRoutes(actor);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x408);
    MonsterInit_SetMotionFlags(actor, 0x40);
    if (Script_GetKeywordValue('E', 0))
        MonsterInit_SetStatusFlags(actor, 0x2000000);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
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
    Octopus_InitControlPresets(actor);
    Octopus_BeginIdleAnimation(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    OctopusInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Octopus_Init(void *object);

void Octopus_Create(void)
{
    u8 *p = Heap_Alloc(0x63c);
    if (p != 0) {
        Memory_Clear(p, 0x63c);
        if (Octopus_Init(p) < 0) {
            Octopus_Destroy(p);
            Heap_Free(p);
        }
    }
}
