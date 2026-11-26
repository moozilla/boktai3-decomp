#include "monsters/lun.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_availability.h"
#include "player/player.h"
#include "engine/ram.h"
#include "global.h"
#include "scene/actor/scene_actor_reaction.h"
#include "npc/manager/scene_actor_animation.h"
#include "scene/motion/scene_actor_terrain.h"
#include "engine/engine.h"
#include "scene/actor/scene_actor_behavior.h"
#include "script/script_utility.h"
#include "scene/motion/scene_actor_route.h"
#include "scene/motion/scene_actor_chase.h"
#include "scene/actor/scene_actor_control_phase.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "scene/actor/scene_actor_placement.h"
#include "engine/engine_globals.h"
#include "monsters/chase_actor.h"
#include "scene/collision/contact_actor.h"
#include "engine/math/movement_curve.h"
#include "engine/video/display_sequence_inline.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "scene/actor/scene_actor_state.h"
#include "scene/collision/scene_body_api.h"
#include "engine/video/render.h"
#include "monsters/scene_actor_init_group.h"
#include "monsters/scene_actor_init_graphics.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_turn.h"
#include "engine/math/math_shift.h"
#include "monsters/ghoul_disc.h"
#include "bosses/vanargand/vanargand_projectiles.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "bosses/vanargand/vanargand_parts.h"

void SceneActorControl_OnIdle(u8 *);

void Player_IncrementActionSpeedBonus(struct Player *player, u32 value);

void Player_QueueStatusEffect(struct Player *player, s32 effect);

void Sound_PlaySong(u32 song); /* m4aSongNumStart, remembering the song per track unless muted */

static inline void Lun_SetState(struct ChaseActor *actor, u32 state, u8 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline u32 Lun_ReadContextFlags(struct ChaseActor *actor, u8 mask)
{
    struct Lun *context = actor->base.context;
    return context->flags3D4 & mask ? 1 : 0;
}

static inline u8 Lun_IsUnavailable(struct ChaseActor *actor)
{
    if (actor) {
        if (SceneActorContact_MotionFlags(actor, 0x40) || SceneActorChase_StatusFlags(actor, 0x20000) ||
            actor->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline u8 Lun_IsPlayerFast(void)
{
    u32 state = gPlayerPassageState;
    u32 result = 0;
    if (state && gPlayerInteractionMode == 5)
        result = 1;
    return result;
}

static inline void Lun_SetContactFlag(struct ChaseActor *actor, u16 bits)
{
    u16 *flags = &actor->base.placementFlags;
    *flags = bits | *flags;
}

static inline void Lun_ClearReactionFlags(struct ChaseActor *actor, u32 bits)
{
    u16 *flags = &actor->base.reactionFlags;
    u32 mask = ~bits;
    *flags &= mask;
}

static inline void Lun_SetContextFlags(struct Lun *context, const u32 *mask)
{
    context->flags3D4 |= *mask;
}

static inline void Lun_ApplyContactFlag(struct Lun *context)
{
    u32 mask = 2;
    Lun_SetContextFlags(context, &mask);
}

#define LUN_READ_CONTACT_FLAG(result_, actor_, mask_)                                              \
    do {                                                                                           \
        (result_) = Lun_ReadContextFlags(actor_, mask_);                                           \
    } while (0)

static inline u32 Lun_CapturePalette(struct ChaseActor *actor, u16 **entry)
{
    u16 *current = &actor->base.paletteOffset;
    u32 value = *current;
    *entry = current;
    return value;
}

/* Applies the palette-dependent player effect after a vertical contact check. */

void Lun_HandlePlayerContact(void *unused, struct ContactCollider *other, void *object)
{
    struct ChaseActor *actor;
    u8 phase;
    u16 *palette;
    s32 limit;
    s32 distance;

    if (other->contactFlags != 1) {
        return;
    }
    actor = object;
    if (Lun_IsUnavailable(actor))
        return;
    distance = actor->base.body.position.y - gPlayers[0]->position.y;
    {
        s32 fast = Lun_IsPlayerFast();
        if (fast || gPlayers[0]->actionMode == 4) {
            limit = 0x3C;
        } else {
            limit = 0x104;
        }
    }
    limit = Ghoul_Abs(limit - distance);
    if (limit > 0x20) {
        return;
    }
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
    phase = 2;
    Lun_SetState(actor, 0, phase, 0);
    EffectCollision_Disable(&actor->base.colliderB);
    EffectCollision_Disable(&actor->base.colliderA);
    Lun_SetContactFlag(actor, 0x800);
    Lun_ClearReactionFlags(actor, 0x180);
    while (Lun_CapturePalette(actor, &palette) == 0) {
        LUN_READ_CONTACT_FLAG(phase, actor, phase);
        Player_IncrementActionSpeedBonus(gPlayers[0], 0);
        Lun_ApplyContactFlag(actor->base.context);
        Sound_PlaySong(0x2AD);
        return;
    }
    LUN_READ_CONTACT_FLAG(phase, actor, phase);
    Lun_ApplyContactFlag(actor->base.context);
    if (*palette == 1) {
        Player_QueueStatusEffect(gPlayers[0], 1);
    } else {
        Player_QueueStatusEffect(gPlayers[0], 2);
    }
}

void SceneActorBehavior_ApplyContactReaction(void *, void *, void *);

void Lun_OnBodyContact(void *source, void *destination, void *actor)
{
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
}

struct ActorParticipantProgress {
    u8 unknown0[0x3D8];
    u8 interval;
    u8 limit;
    u16 accumulated;
    u8 unknown3DC[0xA];
    u8 elapsed, start;
};

s32 Mod(s32, s32);

void SceneActorAnimation_SelectProfileEntry(struct RoomActor *, s32);

void PlayerContact_ApplySyntheticContact(void *, u32, u32, u32, u32, u32);

static inline s32 WrapCounter(s32 n)
{
    return n - 256;
}

void ActorBehavior_TickParticipantProgress(struct ChaseActor *a)
{
    struct ActorParticipantProgress *s = a->base.context;
    s32 delta;
    u8 *elapsed;
    if (SceneActorReaction_TakePrepared(a)) {
        s->start = (*(u8 **)gPlayers)[0x4F4];
        s->accumulated = 0;
    }
    SceneActorAnimation_SelectProfileEntry(a, 14);
    if (Mod(a->base.frameCounter, s->interval) == 0) {
        u16 *mode = &a->base.paletteOffset;
        u32 value = *mode;
        if (!value) {
            u32 first = a->base.attacks.first[0];
            u32 second = a->base.attacks.second[0];
            u32 last = a->base.attacks.flags[0];
            PlayerContact_ApplySyntheticContact((*(u8 **)gPlayers), first, second, 0, value, last);
        } else {
            {
                u32 increment = a->base.attacks.first[0];
                u16 *counter = &s->accumulated;
                *counter = *counter + increment;
            }
            if (s->accumulated > 100) {
                if (*mode == 1)
                    Player_QueueStatusEffect((*(u8 **)gPlayers), 1);
                else
                    Player_QueueStatusEffect((*(u8 **)gPlayers), 2);
                s->accumulated = 0;
            }
        }
    }
    {
        s32 start = s->start;
        s32 now = (*(u8 **)gPlayers)[0x4F4];
        if (now < start) {
            s32 wrapped = WrapCounter(start);
            delta = now - wrapped;
        } else
            delta = now - start;
    }
    elapsed = &s->elapsed;
    if (*elapsed != delta) {
        if (Mod(*elapsed, 4) == 0) {
            u32 mask = 0x20;
            u16 *flags = &a->base.reactionFlags;
            mask |= *flags;
            *flags = mask;
        }
        *elapsed = delta;
    }
    if (s->elapsed > s->limit)
        a->base.stepDone = 1;
}

void Lun_GetDirectionalFrame(u16 a, s32 *b, s32 *c);

static inline void Lun_SelectDirectionalAnimation(struct RoomActor *actor, u32 selection,
                                                  u32 variant, u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct AnimSprite *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->animationIndex;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mode;
            if (flags & 4)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= 4;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

void Lun_UpdateDirectionalPoseNoSelect(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    u32 variant, flags;
    if (actor->field2B2)
        actor->field2B2 = 0;
    Lun_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    Lun_SelectDirectionalAnimation(actor, entry->value, variant, flags);
}

// A bool8 (u8) result from an inline helper keeps the materialised 0/15 and

// the second test; an int result gets jump-threaded away.

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void Lun_OnSpecialAnimation(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectProfileEntry(p, 15);
}

void Lun_UpdateDirectionalPose(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    u32 variant, flags;
    if (actor->field2B2)
        actor->field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    Lun_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    Lun_SelectDirectionalAnimation(actor, entry->value, variant, flags);
}

// fix.py best version (reprinted C)

// Remaining differences include octant-mask lifetime, control initialization and registers.

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

s32 Div(s32, s32);

void CpuSet(const void *, void *, u32);

void Bee_BuildMovementCurve(void *, void *);

void Bee_SubVector(void *, void *, void *);

s32 Bee_GetMovementVariant(void *);

void Player_SubtractEnergy(void *, u32);

void sub_0813D834(void *, u32, u32);

// confidence=low: turns one octant at animation events until reaching the requested heading.

void Lun_TurnTowardSavedDirection(struct ChaseActor *actor)
{
    struct SceneActorBody *motion = &actor->base.body;
    struct DisplaySequenceState *sequence;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        s32 current = ((((s32)actor->base.body.direction) + 0x70) >> 5) & 7;
        u8 desired = actor->base.nextDirection;
        s32 target = ((((s32)desired) + 0x70) >> 5) & 7;
        if (current == target) {
            actor->base.body.direction = desired;
            SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
            actor->base.stepDone = 1;
            return;
        }
        target -= 8;
        {
            s32 k1 = 4;
            if (((u8)((current - target) & 7)) <= k1) {
                SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x10000);
            } else {
                SceneActorChase_SetActorFlags(actor, 0x10000);
            }
        }
    }
    sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    {
        u16 *scale;
        if (SceneActorContact_MotionFlags(actor, 0xA02F)) {
            actor->base.field2C2 = 64;
            scale = &actor->base.field2C2;
        } else {
            u16 *p = &actor->base.field2C2;
            u16 value = *p;
            scale = p;
            if (!value)
                *scale = 64;
        }
        if (!actor->base.drawableKind)
            Anim_SetDurationScale(
                &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
                *scale);
        else
            Anim_SetFrameDurationScale(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
    }
    if (actor->base.frameCounter & 4) {
        u32 variant;
        u32 flags;
        u32 selection;
        struct SceneActorProfileEntry *entry = (void *)SceneActorProfiles_GetEntryAddress(
            actor->base.profileId, actor->base.profileMode, 1);
        SceneActorChase_SetControl(actor, actor->base.onEntry, 2);
        if (SceneActorTurn_ActorFlags(actor, 0x10000))
            motion->direction += 32;
        else
            motion->direction -= 32;
        Lun_GetDirectionalFrame(motion->direction, &variant, &flags);
        sequence->frameCount = 0;
        selection = entry->value;
        Lun_SelectDirectionalAnimation((void *)actor, selection, variant, flags);
    }
}

static inline u16 Flag(struct RoomActor *actor, u16 mask)
{
    u16 *p = &actor->reactionFlags;
    return *p = mask | *p;
}

// confidence=low: selects Lun's waiting animation and marks the wait complete after 60 ticks.

void Lun_UpdateWaitingAnimation(struct RoomActor *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 11);
    u32 variant, flags, selection;
    if (actor->field2B2)
        actor->field2B2 = 0;
    Lun_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    selection = entry->value;
    Lun_SelectDirectionalAnimation(actor, selection, variant, flags);
    if (frame <= 29)
        Flag(actor, 32);
    if (frame > 59)
        actor->stepDone = 1;
}

// confidence=low: resumes pursuit or waiting after the lifted-player penalty window.

void SceneActorControl_ConsumeLiftFlagPenalty(struct ChaseActor *input);

void SceneActorControl_ResetCollisionFlags(struct ChaseActor *input);

void Lun_BeginMoveToPosition(struct ChaseActor *actor, struct ContactPosition *target);

void Lun_UpdateWaitingAnimation(struct RoomActor *actor, s32 frame);

static inline void SetControl(struct ChaseActor *a, void *callback, u32 kind, u32 zero, u8 zero8)
{
    a->base.field2B2 = 1;
    a->base.stepDone = zero8;
    a->base.controlKind = kind;
    a->base.target = callback;
    a->base.field2C8 = zero;
    {
        u32 *flags = &a->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->base.pendingControl = zero;
}

static inline void SetState(struct ChaseActor *a, u32 phase, u32 zero, u8 zero8)
{
    u32 one = 1;
    a->base.state2A8 = zero8;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = one;
    a->base.counter2AC = zero;
    a->base.field2B1 = one;
}

void Lun_ResumeAfterLiftPenalty(struct ChaseActor *input, s32 elapsed)
{
    s32 frame = elapsed;
    u8 zero8;
    if (TakeFlag(&input->base.field2B2))
        SceneActorControl_ConsumeLiftFlagPenalty(input);
    {
        u32 flags = 0x10;
        u16 *p = &input->base.reactionFlags;
        u32 value = *p;
        u16 initialState[1] = {0};
        zero8 = initialState[0];
        flags |= value;
        *p = flags;
    }
    MonsterInit_SetStatusFlags(input, 0x100000);
    if (frame > 30) {
        struct Lun *ctx = input->base.context;
        u16 delay;
        SceneActorControl_ResetCollisionFlags(input);
        delay = ctx->routeDelay;
        if (delay) {
            u32 flags = 0x100;
            u16 *p = &input->base.motionFlags;
            flags = flags | (*p);
            *p = flags;
            ctx->routeDelay = ctx->field442;
            Lun_BeginMoveToPosition(input, &gPlayers[0]->position);
            SetState(input, 2, 0, zero8);
        } else {
            u32 zero = delay;
            SetControl(input, Lun_UpdateWaitingAnimation, 10, zero, zero8);
            SetState(input, 1, zero, zero8);
            input->base.body.direction = input->base.tracking.heading;
            input->base.field2C2 = 64;
        }
    }
}

void SparkParticle_Spawn(void *, u32, void *, s32, s32, s32, s32, s32);

void Lun_ResumeAfterLiftPenalty(struct ChaseActor *input, s32 elapsed);

struct LunFlagWord {
    s16 bits : 16;
};

static inline s32 SetFlag(u16 mask, struct LunFlagWord *p)
{
    return p->bits = mask | p->bits;
}

static inline s32 ClearFlag(struct LunFlagWord *p, u32 mask)
{
    return p->bits = mask & p->bits;
}

static inline u32 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 offset;
    const u16 *entry;
    gRandomIndex = index;
    offset = index * 2;
    entry = (const u16 *)(offset + (u32)RANDOM_TABLE_ADDR);
    return *entry;
}

static inline s32 Quarter(s32 v)
{
    if (v >= 0)
        v >>= 2;
    else
        v = -((-v) >> 2);
    return v;
}

static inline void SetMotionFlags(struct ChaseActor *a, u32 input)
{
    u32 mask = input;
    u32 *p = &a->base.statusFlags;
    *p |= mask;
}

static inline u8 Move(struct ChaseActor *a)
{
    s32 x = Quarter(a->base.reactionStart.x - a->base.reactionEnd.x);
    s32 z = Quarter(a->base.reactionStart.z - a->base.reactionEnd.z);
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    a->base.body.motion.x += x;
    a->base.body.motion.z += z;
    a->base.reactionEnd.x += x;
    a->base.reactionEnd.z += z;
    if (x == 0 && z == 0)
        return 1;
    return 0;
}

// confidence=low: eases Lun to its movement target, then selects the next waiting or pursuit state.

inline static u16 SetAnimationFlags(struct ChaseActor *actor, u32 mask)
{
    u16 *p = &actor->base.placementFlags;
    return *p = mask | (*p);
}

void Lun_ApproachMovementTarget(struct ChaseActor *a, s32 frame)
{
    struct Lun *ctx = a->base.context;
    if (TakeFlag(&a->base.field2B2)) {
        SetFlag(32, (struct LunFlagWord *)&a->base.motionFlags);
        if (a->base.motionFlags & 8)
            Sound_PlaySong(0x12A);
        {
            struct ContactCollider *c = &a->base.colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &a->base.colliderA;
            c->flags |= 4;
        }
        SetAnimationFlags(a, 0x800);
    }
    if ((Mod(frame, 12) == 0) && SceneActorBehavior_MotionBits(8, a)) {
        struct ContactPackedVector range;
        range.x = 20;
        range.y = 20;
        range.z = 20;
        SparkParticle_Spawn(&a->base.body.position, (Rand() & 3) + 1, &range,
                            a->base.body.direction + 128, 10, 6, 2, 2);
    }
    SetMotionFlags(a, 0x100000);
    if (Move(a)) {
        a->base.stepDone = 1;
        ClearFlag((struct LunFlagWord *)&a->base.motionFlags, ~0x120);
        {
            SceneActorChase_ClearWordFlags(&a->base.behaviorFlags, 0x80);
        }
        SetMotionFlags(a, 0x100000);
        ctx->hoverPhase = 0;
        {
            u16 liftFlag = 8;
            if (a->base.motionFlags & liftFlag) {
                MonsterInit_SetState(a, 3, 0, 0);
                SceneActorChase_SetControl(a, Lun_ResumeAfterLiftPenalty, 20);
                ClearFlag((struct LunFlagWord *)&a->base.motionFlags, ~8);
                a->base.field2C2 = 64;
            } else if (ctx->routeDelay) {
                SetFlag(0x100, (struct LunFlagWord *)&a->base.motionFlags);
                ctx->routeDelay = ctx->field442;
                Lun_BeginMoveToPosition(a, &gPlayers[0]->position);
                MonsterInit_SetState(a, 0, 2, 1);
                SceneActorControl_ResetCollisionFlags(a);
            } else {
                a->base.body.direction = a->base.tracking.heading;
                SceneActorChase_SetControl(a, Lun_UpdateWaitingAnimation, 10);
                MonsterInit_SetState(a, 0, 1, 1);
                a->base.body.direction = a->base.tracking.heading;
                a->base.field2C2 = 64;
                SceneActorControl_ResetCollisionFlags(a);
            }
        }
    }
}

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

// confidence=low: Lun defeat callback selects death animation and runs its completion script.

void Lun_UpdateDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        {
            SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x100000);
        }
        {
            struct ContactCollider *c = &actor->base.colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &actor->base.colliderA;
            c->flags |= 4;
        }
        MonsterInit_SetMotionFlags(actor, 0x800);
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
    }
    if (actor->base.restart) {
        actor->base.stepDone = 1;
        SceneActorBehavior_CallTerrainScript(actor);
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
        if (!SceneActorChase_QueryMotionFlagComponent(actor, 0x4000) &&
            (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        MonsterInit_SetState(actor, 1, 0, 2);
    }
}

static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}

static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 255] * speed, 12);
}

static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
}

struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

void Centipede_ResetOscillation(void *);

s32 Callback_InvokeActor(void *, void *);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

void Centipede_SelectWanderRoute(void *);

void Centipede_AdvanceMotionPhase(void *, s32);

void sub_080A9DB0(void *, s32, s32);

void Centipede_UpdateOscillation(void *, s32, s32);

static inline u8 Approach(struct ChaseActor *actor, struct SavedMotion *saved, s32 speed)
{
    u8 *motion = (u8 *)actor + 0x50;
    s16 dx = saved->position.x - *(u16 *)(motion + 12);
    s16 dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *(u16 *)(motion + 20) += Math_ShiftTowardZero(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
        *(u16 *)(motion + 24) += Math_ShiftTowardZero(gUnk_086149C4[motion[9]] * speed, 12);
    }
    return 0;
}

static inline void Lun_UpdateRouteAnimationScale(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
            *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

void Lun_UpdateSway(struct ChaseActor *actor);

struct S0809CB58;

void Lun_SelectRandomRouteEntry(struct S0809CB58 *p);

u8 Lun_StepCurveMotion(struct ChaseActor *actor);

static inline s32 CellIndex(const struct ContactPosition *position, struct ContactPosition **saved)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    *saved = (void *)position;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

// confidence=low: follows saved route points and renews a destination after arrival.

void Lun_FollowRoute(struct ChaseActor *actor)
{
    struct Lun *body = actor->base.context;
    s32 speed;
    struct ContactPosition target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&body->routeMotion);
        body->swayStep = 1;
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Lun_UpdateRouteAnimationScale(actor);
    Lun_UpdateSway(actor);
    if (Approach(actor, (void *)(&body->curveSubject), speed) && Lun_StepCurveMotion(actor)) {
        struct SceneActorRouteCursor *route;
        struct ContactPosition *p;
        struct ContactPosition *saved;
        s32 cell, height;
        u32 kind;
        u16 attr;
        Polar((void *)(&body->routeMotion), actor->base.body.direction, speed);
        route = (void *)(&body->route);
        Lun_SelectRandomRouteEntry(route);
        p = &target;
        p->x = route->point->x;
        p->z = route->point->z;
        cell = CellIndex(p, &saved);
        attr = *SceneActorTerrain_GetCell(cell);
        kind = attr >> 4;
        {
            u32 mask = 15;
            mask &= attr;
            height = mask << 8;
        }
        switch (kind) {
            case 1:
                height -= ((u8 *)p)[4];
                break;
            case 2:
                height -= ((u8 *)p)[0];
                break;
        }
        p->y = height;
        Lun_BeginMoveToPosition(actor, saved);
    }
    actor->base.counter2AC++;
}

// confidence=low: decays saved horizontal route motion before returning to pursuit or rest.

static inline void SetState_DecayRouteMotion(struct ChaseActor *a, u32 phase, u32 behavior,
                                             u32 zero)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}

void Lun_DecayRouteMotion(struct ChaseActor *a)
{
    struct Lun *ctx = a->base.context;
    if (TakeFlag(&a->base.field2B1))
        SceneActorChase_SetControl(a, a->base.onEntry, 2);
    a->base.body.motion.x += ctx->routeMotion.x;
    a->base.body.motion.z += ctx->routeMotion.z;
    if (Mod(a->base.counter2AC, 4) == 0) {
        ctx->routeMotion.x >>= 1;
        ctx->routeMotion.z >>= 1;
    }
    if (a->base.stepDone) {
        u16 flags = SceneActorChase_QueryMotionFlagComponent(a, 0x100);
        if (flags)
            SetState_DecayRouteMotion(a, 2, 1, 0);
        else
            SetState_DecayRouteMotion(a, 0, 2, flags);
    } else
        a->base.counter2AC++;
}

struct LunRouteEntry {
    u8 unknown0[4];
    u8 *data;
    u16 *point;
};

void Lun_SetMovementEndpoints(struct ChaseActor *, struct ContactPosition *);

void Lun_ChooseMovementDestination(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        state[0x3E2] = 0;
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((s16)actor->base.heightOffset > 239 && (s32)actor->base.counter2AC > 60) {
        struct LunRouteEntry *route;

        actor->base.heightOffset = 240;
        route = (struct LunRouteEntry *)(state + 0x42C);
        Lun_SelectRandomRouteEntry(route);
        target = &destination;
        target->x = route->point[0];
        target->z = route->point[1];
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            endpoint = target;
            target->y = Chase_HeightAt(Chase_CellIndex(cellX, cellZ), target);
        }

        Lun_SetMovementEndpoints(actor, endpoint);
        Lun_StepCurveMotion(actor);
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        {
            struct ContactPosition *position = &actor->base.body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        actor->base.nextDirection = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(state + 0x424);
    }
    actor->base.counter2AC++;
}

static inline void Blink(struct ChaseActor *actor)
{
    u32 mask = 0x20;
    mask |= actor->base.reactionFlags;
    actor->base.reactionFlags = mask;
}

static inline void DisableCollider(struct ContactCollider *collider)
{
    u32 mask = ~4;
    collider->flags &= mask;
}

// confidence=low: flashes at increasing frequency, then resets collision and movement state.

void Lun_UpdateTimedRecovery(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 16);
        *(u32 *)(state + 0x3D4) |= 1;
        DisableCollider(&actor->base.colliderB);
        DisableCollider(&actor->base.colliderA);
        {
            u16 *flags = &actor->base.placementFlags;
            u32 mask = ~0x800;
            mask &= *flags;
            *flags = mask;
        }
    }
    if ((s32)actor->base.counter2AC < 40) {
        if (Mod(actor->base.counter2AC, 8) == 0)
            Blink(actor);
    } else if (Mod(actor->base.counter2AC, 3) == 0)
        Blink(actor);
    if ((s32)actor->base.counter2AC > 60) {
        actor->base.heightOffset += actor->base.body.position.y - actor->base.groundHeight;
        SceneActorControl_ResetCollisionFlags(actor);
        SceneActorTerrain_EnterState(actor, 0, 0, 0);
    }
    actor->base.counter2AC++;
}

void Lun_OnSpecialAnimation(u8 *p);

static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}

static inline u32 *ClearFlags(u32 *field, u32 mask)
{
    *field &= mask;
    return field;
}

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

// confidence=low: initializes a random launch vector and ends the action on motion flag 0x10.

void Lun_UpdateLaunchedReaction(struct ChaseActor *actor)
{
    struct Lun *state = actor->base.context;
    u8 entered = SceneActorControl_TakeFlag(&actor->base.field2B1);
    u32 *flags = &actor->base.statusFlags;
    if (entered) {
        u8 angle;
        SceneActorChase_SetControl(actor, Lun_OnSpecialAnimation, 0x24);
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
        angle = Random() & 255;
        ZeroFill8(&state->routeMotion);
        actor->base.secondaryMotion.x = Scale12(gUnk_086149C4[angle] * 12);
        actor->base.secondaryMotion.y = 10;
        actor->base.secondaryMotion.z = Scale12(gUnk_086149C4[(angle + 64) & 255] * 12);
        flags = ClearFlags(&actor->base.statusFlags, ~0x100000);
    }
    SceneActorChase_SetActorFlags(actor, 0x2000);
    {
        u32 mask = 0x10;
        if (*flags & mask) {
            *flags |= 0x100000;
            ZeroFill8(&actor->base.secondaryMotion);
            SceneActorTerrain_EnterState(actor, 0, 0, 4);
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        }
    }
    actor->base.counter2AC++;
}

extern u8 gUnk_08605E34[];

void Lun_InitMotionPreset(u8 *p)
{
    *(u8 **)(p + 0x284) = gUnk_08605E34;
}

/* SceneActor_BeginLinkedIdleReaction_0809C834: SceneActor_BeginLinkedIdleReaction variant with other offsets/flags and without the collider flag clear (provisional name) */

static inline u8 LinkedFlag(struct Lun *p)
{
    u32 mask = 0x2;
    if (p->flags3D4 & mask)
        return 1;
    return 0;
}

void SceneActor_BeginLinkedIdleReaction_0809C834(struct ChaseActor *s)
{
    if (!SceneActor_ActionsBlocked(s) && LinkedFlag(s->base.context)) {
        u32 three = 3;
        u8 *state = &s->base.state2A8;
        u8 zero = 0;
        u8 one;
        *state = zero;
        s->base.phase2A9 = zero;
        s->base.behavior2AA = three;
        s->base.counter2AC = zero;
        {
            u8 *queued = &s->base.field2B1;
            one = 1;
            *queued = one;
        }
        SceneActorChase_SetControl(s, SceneActorControl_OnIdle, 0x10);
    }
}

static inline void SetHeightFromGround(struct ChaseActor *actor)
{
    u32 ground = ((struct SceneActorControlGroundView *)actor)->ground;
    actor->base.body.position.y = actor->base.heightOffset + ground;
}

// confidence=low: adds the hover offset to the actor and bobs its drawable.

void Lun_UpdateHoverHeight(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 delta, magnitude;
    s32 cap;

    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return;
    {
        u32 mask = 1;
        if (SceneActorPlacement_TestValue(state->flags3D4, mask))
            return;
    }
    if ((u8)(actor->base.controlKind - 0x23) <= 1) {
        SetHeightFromGround(actor);
        return;
    }
    if (!SceneActorPlacement_ReadFlags32((void *)actor, 0x100000)) {
        SetHeightFromGround(actor);
        return;
    }
    {
        u32 passageActive;
        u32 mode = gPlayerPassageState;
        passageActive = 0;
        if (mode != 0 && gPlayerInteractionMode == 5)
            passageActive = 1;
        if (passageActive || gPlayers[0]->actionMode == 4)
            cap = 0x3c;
        else
            cap = 0x104;
    }
    if ((s16)actor->base.heightOffset < cap) {
        actor->base.heightOffset += Vanargand_TruncateShift(gUnk_086149C4[state->phase], 9);
        if (state->phase <= 0x3f)
            state->phase++;
        actor->base.body.position.y =
            ((struct SceneActorControlGroundView *)actor)->ground + actor->base.heightOffset;
    }
    delta =
        ((struct SceneActorControlGroundView *)actor)->ground + cap - actor->base.body.position.y;
    magnitude = delta >= 0 ? delta : -delta;
    if (magnitude > 0x10) {
        if (delta < 0)
            actor->base.heightOffset -= 2;
        else
            actor->base.heightOffset += 2;
    }
    SetHeightFromGround(actor);
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
        drawable->position.y += Vanargand_TruncateShift(gUnk_086149C4[state->phase] * 32, 12);
    }
    state->phase += 2;
}

void Lun_UpdateSway(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    if (state->sway > 320) {
        state->swayStep = -1;
        state->sway = 320;
    } else if (state->sway < -320) {
        state->swayStep = 1;
        state->sway = -320;
    }
    state->sway += state->swayStep * 12;
}

u8 *Path_GetPointData(u8 *);

struct S0809CB58 {
    u8 pad0[2];
    u8 index;
    u8 unk3;
    u8 *data;
    u8 *entry;
};

void Lun_SelectRandomRouteEntry(struct S0809CB58 *p)
{
    struct S0809CB58 *q = p;
    u8 n;
    n = Mod(Random(), *p->data);
    if (n == p->index)
        n = Mod(n + 1, *p->data);
    q->index = n;
    q->unk3 = 0;
    q->entry = Path_GetPointData(q->data) + q->index * 8;
}

void Lun_OnBodyContact(void *, void *, void *);

void Lun_HandlePlayerContact(void *unused, struct ContactCollider *other, void *object);

void SceneActorCollision_InitBody(void *, void *, void *, void *, void *);

void Collider_Init(void *, u16, u32, u32, u32, void *, void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void Collider_SetPosition(void *, void *, u32);

void Collider_SetCallback(void *, void *, void *);

void ActorContact_InitializeBodyAndSecondary(struct ChaseActor *a)
{
    struct ContactPackedVector bodySize, bodyOffset, secondarySize, secondaryOffset;
    struct ContactCollider *collider;
    bodySize.x = 0x10;
    bodySize.y = 0x80;
    bodySize.z = 0x10;
    bodyOffset.x = 0;
    bodyOffset.y = 0;
    bodyOffset.z = 0;
    SceneActorCollision_InitBody(a, &bodySize, &bodyOffset, Lun_OnBodyContact, a);
    secondarySize.x = 0x10;
    secondarySize.y = 0x54;
    secondarySize.z = 0x10;
    secondaryOffset.x = 0;
    secondaryOffset.y = 0;
    secondaryOffset.z = 0;
    {
        u32 flags = 0x2003;
        collider = &a->base.colliderB;
        Collider_Init(collider, a->base.body.id, flags, 0, 0x10, &secondarySize, &secondaryOffset);
    }
    Collider_SetAttackProperties(collider, 0, 0, 0, 0, 0);
    Collider_SetPosition(collider, &a->base.body.position, 0);
    Collider_SetCallback(collider, Lun_HandlePlayerContact, a);
}

extern const u32 gLunRouteProgressRates[8];

u32 Lun_GetRouteProgressRate(void *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 phase = state->field3E0 >> 7;
    s16 index = phase;
    if (phase > 7)
        index = 7;
    return gLunRouteProgressRates[index];
}

void Lun_SetMovementEndpoints(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Lun *context = actor->base.context;
    context->curveStart = actor->base.body.position;
    context->curveEnd = *target;
    context->curveTime = 0;
}

u8 Lun_StepCurveMotion(struct ChaseActor *actor)
{
    struct Lun *ctx = actor->base.context;
    struct ContactPosition start, end;
    start = ctx->curveStart;
    start.y = 0;
    end = ctx->curveEnd;
    end.y = 0;
    MovementCurve_InterpolatePosition(&ctx->curveSubject.position, &start, &end, 1, ctx->curveTime,
                                      0, ctx->sway);
    if (ctx->curveTime > 1023)
        return 1;
    ctx->curveTime += 128;
    return 0;
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Lun_InitProfile(u8 *p)
{
    *(u32 *)(p + 0x308) = SceneActorProfiles_FindOrAdd(0x11, 0);
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

// confidence=low: resets movement interpolation and starts the directional animation.

void Lun_ResetMovementState(struct ChaseActor *actor)
{
    struct Lun *context = actor->base.context;
    context->curveTime = 0;
    context->hoverPhase = 0;
    context->swayStep = 1;
    context->sway = 0;
    context->participantElapsed = 0;
    context->participantStart = 0;
    context->flags3D4 = 0;
    ZeroFill8(&context->curveStart);
    ZeroFill8(&context->curveEnd);
    ZeroFill8(&context->routeMotion);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void SceneActorControl_ResetCollisionFlags(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    u32 mask = 0x100000;
    u32 *flags = &actor->base.statusFlags;
    *flags |= mask;
    SceneActorChase_ClearWordFlags(&state->flags3D4, 1);
    {
        struct ContactCollider *colliderB = &actor->base.colliderB;
        u32 mask;
        mask = ~4;
        colliderB->flags = mask & colliderB->flags;
        {
            struct ContactCollider *colliderA = &actor->base.colliderA;
            mask &= colliderA->flags;
            colliderA->flags = mask;
        }
    }
    {
        u16 *p = &actor->base.placementFlags;
        u32 mask;
        mask = 0xFFFFF7FF;
        *p &= mask;
    }
}

void Lun_GetDirectionalFrame(u16 a, s32 *b, s32 *c)
{
    s32 t = ((a + 0x70) >> 5) & 7;
    if (t > 4) {
        *b = 8 - t;
        *c = 1;
    } else {
        *b = t;
        *c = 0;
    }
}

void Lun_BeginMoveToPosition(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Lun *context = actor->base.context;
    struct ContactPosition delta;
    Lun_SetMovementEndpoints(actor, target);
    Lun_StepCurveMotion(actor);
    SceneActorTerrain_EnterState(actor, 0, 0, 1);
    Chase_Delta(&delta, &actor->base.body.position, target);
    actor->base.nextDirection = Math_Atan2(delta.x, delta.z);
    ZeroFill8(&context->routeMotion);
}

struct SceneActorTargetLink {
    u8 *target;
    u8 unknownE0[0xB];
    u8 flags;
};

void SceneActorControl_ResetTargetFlags(struct ChaseActor *input)
{
    struct ChaseActor *s = input;
    struct SceneActorPendingTargetState *q = s->base.context;
    u16 *hp = &s->base.motionFlags;
    u32 hm = 0xFFFFFEFF;
    struct SceneActorTargetLink *t;
    u32 bm;
    u16 zero;
    u16 hv;
    hv = *hp & hm;
    zero = 0;
    *hp = hv;
    t = (void *)((u8 *)s + 0xDC);
    bm = ~8;
    t->flags = t->flags & bm;
    q->field444 = zero;
}

#define RouteCursor_SelectRandomPoint Lun_SelectRandomRouteEntry

#define RouteActor_SetDestination Lun_BeginMoveToPosition

void SceneActorControl_ResetTargetFlags(struct ChaseActor *input);

struct RouteActorDestinationState {
    u8 unknown0[0x42C];
    struct SceneActorRouteCursor route;
    u8 unknown438[12];
    u16 delay;
};

static inline u16 TestFlag(struct ChaseActor *p, u32 mask)
{
    mask &= p->base.motionFlags;
    return mask;
}

void RouteActor_UpdateRandomDestination(struct ChaseActor *actor)
{
    struct RouteActorDestinationState *state = actor->base.context;
    if (!TestFlag(actor, 0x100)) {
        state->delay = 0;
    } else if (state->delay) {
        state->delay--;
    } else {
        struct ContactPosition target;
        struct ContactPosition *p;
        struct SceneActorRouteCursor *route = &state->route;
        s32 cell;
        u16 attr;
        u32 kind;
        s32 height;
        Lun_SelectRandomRouteEntry(route);
        p = &target;
        p->x = route->point->x;
        p->z = route->point->z;
        cell = SceneActorTerrain_GetCellIndex(p);
        attr = *SceneActorTerrain_GetCell(cell);
        kind = attr >> 4;
        {
            u32 mask = 15;
            mask &= attr;
            height = mask << 8;
        }
        switch (kind) {
            case 1:
                height -= ((u8 *)p)[4];
                break;
            case 2:
                height -= ((u8 *)p)[0];
                break;
        }
        p->y = height;
        Lun_BeginMoveToPosition(actor, &target);
        SceneActorControl_ResetTargetFlags((u8 *)actor);
    }
}

static inline u32 TestFlag_ReflectAtRaisedTerrain(struct ChaseActor *p, u32 mask)
{
    mask &= p->base.behaviorFlags;
    return mask;
}

static inline s32 Height(s32 cell, struct ContactPosition *p)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= ((u8 *)p)[4];
            break;
        case 2:
            height -= ((u8 *)p)[0];
            break;
    }
    return height;
}

void RouteActor_ReflectAtRaisedTerrain(struct ChaseActor *actor)
{
    s16 *x_2;
    struct ContactPosition target;
    if (TestFlag_ReflectAtRaisedTerrain(actor, 0x2000)) {
        struct ContactPosition *p = &target;
        struct ContactPosition *position = &actor->base.body.position;
        struct ContactPosition *offset = &actor->base.secondaryMotion;
        s32 cell, height;
        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        cell = SceneActorTerrain_GetCellIndex(&target);
        height = Height(cell, &target);
        if (actor->base.groundHeight < height) {
            s32 zero;
            s32 off = 0x2EC;
            s16 *x = (s16 *)((u8 *)actor + off);
            s32 value = -*x;
            zero = 0;
            *x = value;
            off = 0x2F0;
            x_2 = (s16 *)(((u8 *)actor) + off);
            value = -(*x_2);
            *x_2 = value;
            actor->base.body.motion.x = zero;
            actor->base.body.motion.z = zero;
        }
    }
}

u32 Lun_OnTargetCheck(struct ChaseActor *actor)
{
    struct SceneActorChaseTracking *p;
    s32 distance;
    if (!MonsterInit_TestTargetingFlags(&actor->base.tracking, 2))
        goto zero;
    p = &actor->base.tracking;
    {
        s32 mask = -3;
        mask &= p->flags;
        p->flags = mask;
    }
    distance = actor->base.body.position.y -
               ((struct SceneActorChasePlayer *)gPlayers[0])->target.position.y;
    if (distance < 0)
        distance = -distance;
    if (distance > 0x200)
        goto zero;
    {
        u32 *a = &actor->base.tracking.distance;
        u32 *b = &actor->base.tracking.limit;
        if (*a >= *b)
            goto zero;
    }
    goto one;
zero:
    return 0;
one: {
    u32 mask = 0x80;
    u16 *flags = &actor->base.reactionFlags;
    mask |= *flags;
    *flags = mask;
}
    {
        u32 mask = 0x10;
        mask |= p->flags;
        p->flags = mask;
    }
    return 1;
}

void Lun_EnterIdleOrCopyCell(struct ChaseActor *s)
{
    u32 mask;
    struct Lun *g;
    u16 zero;
    u16 zero_2;
    g = s->base.context;
    mask = 0x100;
    zero_2 = s->base.motionFlags;
    zero = zero_2 & mask;
    if (zero) {
        u16 v = g->field442;
        g->routeDelay = v;
    } else {
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u8 one;
        u32 *flags;
        u8 *taken;
        struct SceneActorChaseTracking *tracking;
        taken = &s->base.field2B2;
        one = 1;
        *taken = one;
        s->base.stepDone = zero;
        s->base.controlKind = code;
        s->base.target = fn;
        s->base.field2C8 = zero;
        flags = &s->base.pendingControlFlags;
        {
            u32 clear = -2;
            *flags &= clear;
        }
        s->base.pendingControl = zero;
        s->base.state2A8 = 0;
        s->base.phase2A9 = one;
        s->base.behavior2AA = 0;
        s->base.counter2AC = zero;
        s->base.field2B1 = one;
        tracking = &s->base.tracking;
        {
            u32 bit = 8;
            bit |= tracking->flags;
            tracking->flags = bit;
        }
    }
}

static inline u8 LiftFlag(struct SceneActorControlMotionState *state)
{
    u32 mask = 2;
    return Test(state->flags3D4, mask);
}

void SceneActorControl_ConsumeLiftFlagPenalty(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    if (!LiftFlag((void *)SceneActorControl_GetMotionState(actor)))
        return;
    if (actor->base.paletteOffset == 0) {
        gPlayers[0]->movementSpeedReduction--;
    }
    {
        u32 *flags = &SceneActorControl_GetMotionState(actor)->flags3D4;
        u32 clearMask = ~2;
        *flags &= clearMask;
    }
}

void Lun_UpdateHoverHeight(struct ChaseActor *actor);

void RouteActor_UpdateRandomDestination(struct ChaseActor *actor);

u32 Lun_UpdateMovement(u8 *p)
{
    Lun_UpdateHoverHeight(p);
    RouteActor_UpdateRandomDestination(p);
    return 1;
}

void RouteActor_ReflectAtRaisedTerrain(struct ChaseActor *actor);

u32 Lun_UpdateTerrainBounce(struct ChaseActor *actor)
{
    RouteActor_ReflectAtRaisedTerrain(actor);
    return 1;
}

// confidence=low: follows the current route segment and renews its endpoint at the player.

void Lun_RenewPlayerRoute(struct ChaseActor *actor)
{
    struct Lun *body = actor->base.context;
    s32 speed;
    struct ContactPosition delta;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&body->routeMotion);
        body->swayStep = 1;
    }
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Lun_UpdateRouteAnimationScale(actor);
    Lun_UpdateSway(actor);
    if (Approach(actor, (void *)(&body->curveSubject), speed) && Lun_StepCurveMotion(actor)) {
        Lun_SetMovementEndpoints(actor, &gPlayers[0]->position);
        Lun_StepCurveMotion(actor);
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        Chase_Delta(&delta, &actor->base.body.position, (void *)(((u8 **)gPlayers)[0] + 0x30));
        actor->base.nextDirection = Math_Atan2(delta.x, delta.z);
    }
    actor->base.counter2AC++;
}

// confidence=low: follows player displacement while the participant-progress control runs.

void ActorBehavior_TickParticipantProgress(struct ChaseActor *a);

static inline void SetState_FollowParticipantDisplacement(struct ChaseActor *a, u32 phase,
                                                          u32 behavior, u32 zero)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}

void Lun_FollowParticipantDisplacement(struct ChaseActor *a)
{
    struct Lun *ctx = a->base.context;
    struct ContactPosition delta;
    if (TakeFlag(&a->base.field2B1)) {
        SceneActorChase_SetControl(a, ActorBehavior_TickParticipantProgress, 0x23);
        ctx->curveStart = *(struct ContactPosition *)&gPlayers[0]->position;
        SceneActorControl_ResetTargetFlags(a);
    }
    Chase_Delta(&delta, &ctx->curveStart, (void *)&gPlayers[0]->position);
    a->base.body.motion.x += delta.x;
    a->base.body.motion.z += delta.z;
    ctx->curveStart = *(struct ContactPosition *)&gPlayers[0]->position;
    if (a->base.stepDone) {
        SetState_FollowParticipantDisplacement(a, 0, 3, 0);
        SceneActorChase_SetControl(a, SceneActorControl_OnIdle, 0x10);
    } else {
        a->base.counter2AC++;
    }
}

extern u8 gUnk_08605E68[];

void Lun_InitSecondaryActionTable(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605E68;
}

struct RouteActorDestinationStateUpdatePlayerDestinationPhase {
    u8 unknown0[0x442];
    u16 delayDuration, delay;
};

static inline u16 SetFlag_UpdatePlayerDestinationPhase(struct ChaseActor *actor, u32 mask)
{
    u16 *flags = &actor->base.motionFlags;
    return *flags = mask | *flags;
}

void RouteActor_UpdatePlayerDestinationPhase(struct ChaseActor *actor)
{
    if (TakeFlag(&actor->base.field2B1)) {
        struct RouteActorDestinationStateUpdatePlayerDestinationPhase *state = actor->base.context;
        SceneActorChase_SetControl(actor, Lun_UpdateWaitingAnimation, 10);
        SetFlag_UpdatePlayerDestinationPhase(actor, 0x100);
        state->delay = state->delayDuration;
    }
    if (actor->base.stepDone)
        Lun_BeginMoveToPosition(actor, &gPlayers[0]->position);
    else
        actor->base.counter2AC++;
}

void Lun_ChooseDelayedDestination(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((s32)actor->base.counter2AC > 60) {
        struct LunRouteEntry *route;

        route = (struct LunRouteEntry *)(state + 0x42C);
        Lun_SelectRandomRouteEntry(route);
        target = &destination;
        target->x = route->point[0];
        target->z = route->point[1];
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            endpoint = target;
            target->y = Chase_HeightAt(Chase_CellIndex(cellX, cellZ), target);
        }

        Lun_SetMovementEndpoints(actor, endpoint);
        Lun_StepCurveMotion(actor);
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        {
            struct ContactPosition *position = &actor->base.body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        actor->base.nextDirection = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(state + 0x424);
    } else
        actor->base.counter2AC++;
}

extern u8 gUnk_08605E70[];

void Lun_InitReactionTable(u8 *p)
{
    *(u8 **)(p + 0x288) = gUnk_08605E70;
}

static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->motionFlags = mask | s->motionFlags;
}

s32 Lun_PlayResponseAndResetState(struct RoomActor *s)
{
    if (!s->reactionSoundVariant)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (s->hitReactionFlags & 0x40000)
        SetFlags(s, 0x20);
    {
        SceneActorChase_SetControl(s, s->onReaction, 0x12);
        SceneActorChase_SetState(s, 3, 0, 0);
        SceneActorChase_AndWordFlags(&s->statusFlags, 0xFEFFFFFF);
        SceneActorPlacement_ClearFlags(&s->reactionFlags, 0xFFFFF636);
    }
    return 1;
}

u32 Lun_UpdateMovement(u8 *);

void SceneActorControl_DispatchControl(u8 *);

u32 Lun_UpdateTerrainBounce(struct ChaseActor *);

u32 Lun_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    Lun_UpdateMovement(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    Lun_UpdateTerrainBounce(p);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Lun_OnPlayerTargetInit(u8 *p)
{
    *(u8 **)(p + 0xdc) = (*(u8 **)gPlayers) + 0x24;
    SceneActor_UpdateActiveCallbacks(p);
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

u32 Lun_Destroy(struct ChaseActor *s)
{
    u8 *p = s->base.drawable;
    u8 *q;
    u32 m;
    u32 v;
    if (s->base.drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(&((struct SceneActorAnimationTileDrawable *)p)->tile);
    Heap_Free(p);
    m = 0x100;
    if (SceneActorChase_TestField(&s->base.placementFlags, m))
        SceneMessages_UnregisterReceiver(s->base.receiver);
    {
        u8 *r = s->base.shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = s->base.statusFlags & m;
    q = (void *)&s->base.body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&s->base.colliderA);
    SceneActorRegistry_UnlinkActor(s);
    return 0;
}

typedef void (*MonsterInitCommandHandler)(void *actor, const u16 *command);

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void Lun_InitProfile(u8 *p);

u32 Lun_OnUpdate(u8 *p);

u32 Lun_GetRouteProgressRate(void *);

void Lun_TurnTowardSavedDirection(struct ChaseActor *actor);

void Lun_ReadSpawnScript(void *);

void Lun_UpdateDirectionalPoseNoSelect(struct RoomActor *actor);

void Lun_UpdateDirectionalPose(struct RoomActor *actor);

void Lun_ApproachMovementTarget(struct ChaseActor *a, s32 frame);

void Lun_UpdateDefeat(struct ChaseActor *actor);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolveDamage(void);

s32 Lun_PlayResponseAndResetState(struct RoomActor *s);

void SceneActorReaction_SinkAndRemove(void);

void Lun_ResetMovementState(struct ChaseActor *actor);

void Lun_EnterIdleOrCopyCell(struct ChaseActor *s);

u32 Lun_OnTargetCheck(struct ChaseActor *actor);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

u32 Lun_OnPlayerTargetInit(u8 *p);

u32 Lun_Destroy(struct ChaseActor *s);

void AnimData_SelectPaletteBank(void *, s32);

void ActorContact_InitializeBodyAndSecondary(struct ChaseActor *a);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void Lun_InitMotionPreset(u8 *p);

void Lun_InitSecondaryActionTable(u8 *p);

void SceneActorControl_SetStoredMotionCallbacks(void *);

void SceneActorControl_RestartPositionCurve(struct ChaseActor *);

void ActorShadow_Disable(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern MonsterInitCommandHandler const gUnk_08605E78[];

extern MonsterInitCommandHandler const gUnk_086108B0[];

u32 Route_GetCurrentPointFacing(void *);

void Lun_InitReactionTable(u8 *p);

#define SV(d) MonsterInit_ReadWord(d)

static inline void MonsterInit_InitializeBody(struct ChaseActor *arg0)
{
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, r4v, r6v, r3v;
    s32 initialHealth, scriptVariant;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = Script_GetKeywordValue(0x4D, 0);
    ZeroFill8(&origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->base.tracking);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = MonsterInit_ReadWord(0x12C);
        r4v = MonsterInit_ReadWord(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = (u16)Script_GetKeywordValue(0x46, 0);
    arg0->base.scriptId = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        arg0->base.defaultHealth = MonsterInit_ReadHalfword(0);
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    initialHealth = MonsterInit_GetInitialHealth(arg0);
    r3v = r4v;
    arg0->base.motionFlags = 0;
    arg0->base.target = SceneActorControl_OnIdle;
    arg0->base.field2B2 = 1;
    arg0->base.stepDone = 0;
    arg0->base.health = MonsterInit_ClampHealth(initialHealth);
    arg0->base.attachmentTimer = 0;
    if (r3v <= 0)
        r3v = r6v;
    arg0->base.attachmentLimit = r6v;
    arg0->base.attachmentRestart = r3v;
    arg0->base.regionId = 0;

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&arg0->base.hudKind, Script_GetKeywordValue(0x4B, 2));
    scriptVariant = (s16)Script_GetKeywordValue(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    MonsterInit_WriteHalfword(&arg0->base.hudLabel, scriptVariant);
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->base.drawableKind ? 0 : arg0->base.drawable);
    SceneBody_SetMovementProbe(body, arg0->base.movementProbe, 0x40, 0x40);
}

static inline void Lun_ReadPalette(struct ChaseActor *actor, u16 base, u16 alternate)
{
    s32 r1v;
    actor->base.paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0x91);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->base.drawable + 0x2C, r1v);
    else
        *(u8 **)(actor->base.drawable + 0x68) = gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette((void *)(actor->base.drawable + 0x20),
                                     SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct ChaseActor *actor, void *cb0,
                                              u32 (*cb1)(void *), void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.onUpdate = cb0;
    MonsterInit_WriteAnimationQuery(&actor->base.animationQuery.word, cb1);
    actor->base.onEntry = cb2;
    actor->base.onAlternateEntry = cb3;
    actor->base.onStanding = cb4;
    actor->base.onReaction = cb5;
    actor->base.onDefeat = cb6;
    actor->base.onHidden = hidden;
    actor->base.defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_086108B0);
    actor->base.routeReset = 0;
    actor->base.targetQuery = targetQuery;
    actor->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    actor->base.onDamage = damage;
    actor->base.onContact = contact;
    actor->base.field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Lun_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Lun_OnPlayerTargetInit;
    actor->base.destroy = (SceneActorResultCallback)Lun_Destroy;
}

static inline void Lun_InstallRoutes(struct ChaseActor *actor)
{
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_08605E78);
    actor->base.routeReset = Lun_ResetMovementState;
    actor->base.reactionGate = Lun_EnterIdleOrCopyCell;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, Lun_OnTargetCheck);
}

static inline u32 Lun_ReadRoute(struct ChaseActor *actor, struct Lun *lun,
                                struct SceneActorRegistryRoute *route)
{
    struct ContactPosition point;
    route->activeBranch = 0;
    if (Script_SeekToKeyword('r')) {
        s32 pointId;
        route->routeId = MonsterInit_ReadWord(0);
        route->parameterB = 0;
        pointId = MonsterInit_ReadWord(0);
        route->pointId = pointId;
        route->currentPoint = pointId;
        route->branches[MonsterInit_RouteActiveBranch(route)] = route->routeId;
        route->branches[MonsterInit_RouteAlternateBranch(route)] =
            Script_GetPc() ? Script_GetValue() : MonsterInit_RouteId(route);
        Path_GetPointPositionById(&point, MonsterInit_RouteId(route),
                                  MonsterInit_RoutePointId(route));
        actor->base.terrainCell = MonsterInit_CellIndex(&point);
        point.y = actor->base.groundHeight =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &point);
        MonsterInit_BeginRoute(&actor->base.body, route);
        return 1;
    } else {
        route->routeId = 0;
        route->parameterB = 0;
        route->pointId = 0;
        route->currentPoint = 0;
        lun->route.branches[MonsterInit_RouteActiveBranch(route)] = 0;
        lun->route.branches[MonsterInit_RouteAlternateBranch(route)] = 0;
    }
    return 0;
}

static inline void Lun_ReadPosition(struct ChaseActor *actor, struct Lun *lun)
{
    if (Script_SeekToKeyword('p')) {
        s32 ground;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        actor->base.terrainCell = MonsterInit_CellIndex(&actor->base.body.position);
        ground = MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor),
                                           &actor->base.body.position);
        actor->base.groundHeight = ground;
        lun->ground3DC = ground;
        MonsterInit_DrawablePosition(actor);
    }
}

s32 Lun_Init(void *object)
{
    struct Lun *lun = object;
    struct ChaseActor *actor = object;
    struct ContactPosition tmp;
    u32 flag;
    s32 r1v;
    lun->flags3D4 = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_LUN);
    Lun_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xD28E, 0x9607);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        ZeroFill8(&tmp);
        res = ScriptProc_Start(0xCB05, 0xD28E);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor);
    {
        struct SceneActorRegistryRoute *route = &lun->route;
        if (Lun_ReadRoute(actor, lun, route)) {
            MonsterInit_SnapToRoute(&actor->base.body.position, route);
            MonsterInit_DrawablePosition(actor);
            actor->base.body.direction = Route_GetCurrentPointFacing(route);
        }
    }
    Lun_ReadPosition(actor, lun);
    if (Script_SeekToKeyword('P')) {
        lun->parameter3D8 = MonsterInit_ReadByte(0);
        lun->parameter3D9 = MonsterInit_ReadByte(0);
    }
    lun->field442 = Script_GetKeywordValue('l', 0);
    Lun_ReadPalette(actor, 0x8C, 0x192);

    ActorContact_InitializeBodyAndSecondary(actor);
    MonsterInit_InstallPrimary(actor, Lun_OnUpdate, Lun_GetRouteProgressRate,
                               Lun_TurnTowardSavedDirection, Lun_UpdateDirectionalPoseNoSelect,
                               Lun_UpdateDirectionalPose, 0, Lun_ApproachMovementTarget,
                               Lun_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorReaction_ResolveDamage, Lun_PlayResponseAndResetState);
    Lun_InstallRoutes(actor);
    MonsterInit_SetMotionFlags(actor, 0x60);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100400);
    if (MonsterInit_GetPaletteOffset(actor) != 0)
        MonsterInit_SetStatusFlags(actor, 8);
    SceneActorBehavior_InitializeShadow(actor);
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
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Lun_InitMotionPreset(actor);
    Lun_InitSecondaryActionTable(actor);
    Lun_InitReactionTable(actor);
    actor->base.field2E8 = 2;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Lun_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Lun_Init(void *);

void Lun_Create(void)
{
    void *m = Heap_Alloc(0x44c);
    if (m != 0) {
        Memory_Clear(m, 0x44c);
        if (Lun_Init(m) < 0) {
            Lun_Destroy(m);
            Heap_Free(m);
        }
    }
}
