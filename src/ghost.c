#include "ghost.h"

#include "global.h"

#include "scene_actor_reset.h"

#include "player.h"

#include "scene_actor_state.h"

#include "scene_actor_chase_reaction.h"

#include "scene_actor_terrain.h"

#include "scene_actor_animation.h"

#include "engine.h"

#include "ram.h"

#include "scene_actor_control_phase.h"

#include "scene_actor_chase_shared.h"

#include "script_utility.h"

#include "scene_actor_route.h"

#include "scene_actor_visibility.h"

#include "scene_actor_control_helpers.h"

#include "engine_globals.h"

#include "contact_actor.h"

#include "movement_curve.h"

#include "scene_actor_control_motion.h"

#include "display_sequence_inline.h"

#include "actor_animation_controller.h"

#include "debug.h"

#include "scene_actor_behavior.h"

#include "scene_route_sample.h"

#include "monster_particle_random.h"

#include "input.h"

#include "gba/io_reg.h"

#include "scene_body_api.h"

#include "render.h"

#include "game_state.h"

#include "scene_actor_init_group.h"

#include "scene_actor_init_graphics.h"

/* Empty callback; callers have not been identified. */

void Ghost_NoOp(void)
{
}

static inline u8 Ghost_HasPlayerLink(struct Ghost *p)
{
    u32 mask = 0x80;
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline void Ghost_EnableCollider(struct ContactCollider *c)
{
    c->flags |= 4;
}

// confidence=low: contact type 1 interrupts control and starts a linked-player reaction.

void Ghost_BeginContactReaction(void *unused, struct ContactCollider *contact,
                                struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *s = input;
    if (!SceneActor_ActionsBlocked(s)) {
        u32 mask = 0x4000;
        u16 zero;
        zero = s->flags2D4;
        zero &= mask;
        if (!zero && contact->contactFlags == 1) {
            struct Ghost *p = s->context;
            void *fn = SceneActorControl_OnIdle;
            u8 code = 0x10;
            u8 one = 1;
            u8 zero8;
            u32 *flags;
            u32 two;
            u8 linked;

            s->field2B2 = one;
            s->field2B0 = zero;
            s->field2B5 = code;
            s->target = fn;
            zero8 = 0;
            s->field2C8 = zero;
            flags = &s->flags114;
            {
                u32 mask = -2;
                *flags &= mask;
            }
            s->word104 = zero;
            two = 2;
            s->state2A8 = zero8;
            s->phase2A9 = two;
            s->behavior2AA = zero8;
            s->counter2AC = zero;
            s->field2B1 = one;
            Ghost_EnableCollider(&s->colliderB);
            Ghost_EnableCollider(&s->colliderA);
            {
                u16 *f = &s->flags2D6;
                u32 mask = ~0x180;
                *f &= mask;
            }
            /* The linked flag is read here, but the result goes unused. */
            linked = Ghost_HasPlayerLink(p);
            {
                struct Player *player = gPlayers[0];
                u32 three;
                three = 3;
                player->movementSpeedReduction += 3;
                {
                    u32 mask = 0x80;
                    p->flags |= mask;
                }
                p->revealTicks = three;
            }
            Sound_PlaySong(0x3CB);
        }
    }
}

/* SceneActorBehavior_OnContactForward: forwards the contact unless the actor is unavailable or flagged */

void SceneActorBehavior_ApplyContactReaction(void *, void *);

void SceneActorBehavior_OnContactForward(u16 *source, void *destination,
                                         struct SceneActorRegistryResetView *actor)
{
    if (SceneActor_ActionsBlocked(actor))
        return;
    {
        u32 mask = 0x4000;
        u16 *f = &actor->flags2D4;
        if (*f & mask)
            return;
    }
    {
        u32 mask = 0x800;
        if (source[3] & mask)
            return;
    }
    SceneActorBehavior_ApplyContactReaction(source, destination);
}

extern const s16 gUnk_086149C4[];

s32 Div(s32, s32);

void CpuSet(const void *, void *, u32);

void Bee_BuildMovementCurve(void *, void *);

void Bee_SubVector(void *, void *, void *);

s32 Bee_GetMovementVariant(void *);

s32 Mod(s32, s32);

void Player_SubtractEnergy(void *, u32);

void sub_0813D834(void *, u32, u32);

static inline void Ghost_SelectDirectionalAnimation(struct SceneActorAnimationView *actor,
                                                    u32 selection, u32 variant, u32 flags)
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
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->selection;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

static inline u16 Ghost_SetEffectFlags(struct SceneActorAnimationView *a, u16 mask)
{
    u16 *p = &((struct SceneActorRegistryResetView *)a)->flags2D6;
    return *p = mask | *p;
}

static inline s32 Ghost_UnwrapProgressCounter(s32 n)
{
    return n - 256;
}

// confidence=low: drains player energy while tracking the player's wrapping progress counter.

void Ghost_UpdateEnergyDrainAnimation(struct SceneActorAnimationView *actor)
{
    struct Ghost *body = actor->link;
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    u32 variant, flags, selection;
    s32 delta;
    u8 *elapsed;
    if (SceneActorControl_TakeFlag(&actor->directionalEntryFlag)) {
        {
            u32 value = gPlayers[0]->facingChangeEffects;
            u8 *p = &body->drainStart;
            u32 zero = 0;
            *p = value;
            body->drainCounter = zero;
        }
        ((struct SceneActorRegistryResetView *)actor)->field2C2 = 16;
    }
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
    if (((struct SceneActorRegistryResetView *)actor)->paletteOffset == 0) {
        if (Mod(((struct SceneActorRegistryResetView *)actor)->field2CA, body->drainInterval) == 0)
            Player_SubtractEnergy(((u8 **)gPlayers)[0],
                                  ((struct SceneActorRegistryResetView *)actor)->attacks.first[0]);
    } else if (((struct SceneActorRegistryResetView *)actor)->paletteOffset == 1) {
        if (Mod(((struct SceneActorRegistryResetView *)actor)->field2CA, body->drainInterval) == 0)
            Player_SubtractEnergy(((u8 **)gPlayers)[0],
                                  ((struct SceneActorRegistryResetView *)actor)->attacks.first[0]);
        if (((u8 **)gPlayers)[0])
            sub_0813D834(((u8 **)gPlayers)[0], 1, 900);
    }
    {
        s32 start = body->drainStart, now = gPlayers[0]->facingChangeEffects;
        if (now < start) {
            s32 wrapped = Ghost_UnwrapProgressCounter(start);
            delta = now - wrapped;
        } else
            delta = now - start;
    }
    elapsed = &body->drainElapsed;
    if (*elapsed != delta) {
        if (Mod(*elapsed, 4) == 0)
            Ghost_SetEffectFlags(actor, 32);
        *elapsed = delta;
    }
    if (body->drainElapsed > body->drainDuration)
        actor->unknown2B0 = 1;
}

void Ghost_UpdateDirectionalPose(struct SceneActorAnimationView *actor)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (actor->directionalEntryFlag)
        actor->directionalEntryFlag = 0;
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
}

void Ghost_UpdateDirectionalPoseB(struct SceneActorAnimationView *actor)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (SceneActorControl_TakeFlag(&actor->directionalEntryFlag))
        ((struct SceneActorRegistryResetView *)actor)->field2C2 = 0x40;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
}

// fix.py best version (reprinted C)

// Remaining differences include octant-mask lifetime, control initialization and registers.

static inline u32 Ghost_TestActionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

// confidence=low: turns one octant at animation events until reaching the requested heading.

void Ghost_TurnTowardHeading(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorBody *motion = &actor->body;
    struct ActorAnim *sequence;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        s32 current = ((((s32)actor->body.direction) + 0x70) >> 5) & 7;
        u8 desired = actor->unknown3CD[0];
        s32 target = ((((s32)desired) + 0x70) >> 5) & 7;
        if (current == target) {
            actor->body.direction = desired;
            SceneActorChase_SetControl(actor, actor->savedTarget, 0);
            actor->field2B0 = 1;
            return;
        }
        target -= 8;
        {
            s32 k1 = 4;
            if (((u8)((current - target) & 7)) <= k1) {
                u32 *p = &actor->flags2CC;
                u32 mask = ~0x10000;
                *p &= mask;
            } else {
                u32 mask = 0x10000;
                actor->flags2CC |= mask;
            }
        }
    }
    sequence = &((struct GhostVisual *)actor->drawable)->sequence.anim;
    {
        u32 mask = 0xA02F;
        u16 *scale;
        if (actor->flags2D4 & mask) {
            actor->field2C2 = 64;
            scale = &actor->field2C2;
        } else {
            u16 *p = &actor->field2C2;
            u16 value = *p;
            scale = p;
            if (!value)
                *scale = 64;
        }
        if (!actor->drawableKind)
            Anim_SetDurationScale(&((struct GhostVisual *)actor->drawable)->sequence.anim, *scale);
        else
            Anim_SetFrameDurationScale(
                &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
    }
    if (actor->field2CA & 4) {
        u32 variant;
        u32 flags;
        u32 selection;
        SceneActorChase_SetControl(actor, actor->entryTarget224, 2);
        if (Ghost_TestActionFlags(actor, 0x10000))
            motion->direction += 32;
        else
            motion->direction -= 32;
        Ghost_GetDirectionalFrame(motion->direction, &variant, &flags);
        sequence->count = 0;
        selection =
            *((u8 *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1));
        Ghost_SelectDirectionalAnimation((void *)actor, selection, variant, flags);
    }
}

static inline u16 Ghost_SetAnimationFlags(struct SceneActorAnimationView *actor, u16 mask)
{
    u16 *p = &((struct SceneActorRegistryResetView *)actor)->flags2D6;
    return *p = mask | *p;
}

// confidence=low: selects Ghost's waiting animation and marks the wait complete after 60 ticks.

void Ghost_UpdateWaitingAnimation(struct SceneActorAnimationView *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (actor->directionalEntryFlag)
        actor->directionalEntryFlag = 0;
    Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 11);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
    if (frame <= 29)
        Ghost_SetAnimationFlags(actor, 32);
    if (frame > 59)
        actor->unknown2B0 = 1;
}

// confidence=low: recoil callback selects animation 15 and completes after sixty ticks.

void Ghost_UpdateRecoilAnimation(struct SceneActorRegistryResetView *actor, s32 tick)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    }
    if (tick <= 29) {
        SceneActorChase_SetReactionFlags(actor, 32);
    }
    if (tick > 59) {
        actor->field2B0 = 1;
    }
}

void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);

static inline s32 Ghost_QuarterMotion(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}

static inline s32 Ghost_ClampMotion(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}

static inline u8 Ghost_IsMotionAtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline u32 Ghost_TestMotionFlags(u16 *f, u32 maskInput)
{
    u32 mask = maskInput;
    return *f & mask;
}

static inline void Ghost_SetStatePhase(struct SceneActorRegistryResetView *actor, u8 state,
                                       u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u32 Ghost_RandomSparkCount(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)RANDOM_TABLE_ADDR + index);
    v &= 3;
    return v + 1;
}

// confidence=low: damage stagger; sparks while flagged, then converges saved motion and returns to idle.

void Ghost_UpdateHitStagger(struct SceneActorRegistryResetView *actor, s32 tick)
{
    struct Ghost *ctx = actor->context;
    struct ContactPosition range;
    s32 x, z;
    s16 *priorX, *priorZ;
    u16 *f;
    u8 taken;
    taken = SceneActorControl_TakeFlag(&actor->field2B2);
    f = &actor->flags2D4;
    if (taken) {
        {
            u32 value = 0x20;
            value |= *f;
            *f = value;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        Ghost_ClearActionFlags(actor);
        SceneActor_EndLinkedReaction(actor);
        if (Ghost_TestMotionFlags(f, 8))
            Sound_PlaySong(0x12A);
    }
    if (Ghost_TestMotionFlags(f, 8)) {
        if (Mod(tick, 12) == 0) {
            range.x = 60;
            range.y = 60;
            range.z = 40;
            SparkParticle_Spawn(&actor->body.position, Ghost_RandomSparkCount(), &range,
                                actor->body.direction + 128, 10, 6, 2, 2);
        }
    }
    {
        s32 current = actor->reactionStart.x;
        s16 *field = &actor->reactionEnd.x;
        s32 difference = current - *field;
        priorX = field;
        x = Ghost_QuarterMotion(difference);
    }
    {
        s32 current = actor->reactionStart.z;
        s16 *field = &actor->reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = Ghost_QuarterMotion(difference);
    }
    x = Ghost_ClampMotion(x);
    z = Ghost_ClampMotion(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (Ghost_IsMotionAtRest(x, z)) {
        actor->field2B0 = 1;
        Ghost_EndPlayerSlowdown(actor);
        {
            struct GhostSignedFlags16 *flags = (void *)f;
            s32 m = ~0x28;
            m &= (u16)flags->bits;
            flags->bits = m;
        }
        {
            u32 *flags = &actor->flags2CC;
            s32 mask = ~0x80;
            *flags &= mask;
        }
        {
            u32 mask = 0x100000;
            actor->flags2D0 |= mask;
        }
        SceneActor_EndLinkedReaction(actor);
        {
            u32 mask = 0x10;
            u32 *flags = &ctx->flags;
            *flags |= mask;
        }
        {
            u16 v = ctx->delayDuration;
            ctx->delay = v;
        }
        Ghost_SetStatePhase(actor, 0, 2, 2);
    }
}

void *BestiaryDisplay_GetEntryForActor(u32, u32);

void GameState_SetEventFlag(void *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

void EffectManager_SpawnParticlesAt(u32, u32, u32, void *, void *, u32, u32);

static inline u32 Ghost_ReadDefeatFlags(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D4 & mask;
}

static inline void Ghost_SetScriptValue(s32 value, s32 *entry)
{
    *entry = value;
}

// confidence=low: defeat control; emits debris sprites, runs the defeat script, then returns to idle.

void Ghost_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *ctx = actor->context;
    struct {
        u16 x;
        u16 y;
        u16 z;
        u16 w;
    } pieces[3];
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        actor->colliderA.flags |= 4;
        if (Ghost_ReadDefeatFlags(actor, 8))
            Sound_PlaySong(0x12A);
        Ghost_EndPlayerSlowdown(actor);
        if (!Ghost_ReadDefeatFlags(actor, 0x4000)) {
            s32 i;
            struct ContactPosition *pos;
            u32 index = (gRandomIndex + 1) & 1023;
            u32 count;
            gRandomIndex = index;
            count = Mod(*((u16 *)(0x0203B400 + (index * 2))), 3) + 1;
            pos = &actor->body.position;
            {
                u16 *x = (u16 *)&pos->x;
                u16 *y = (u16 *)&actor->body.position.y;
                u16 *z = (u16 *)&actor->body.position.z;
                for (i = 0; i < 3; i++) {
                    pieces[i].x = *x;
                    pieces[i].y = *y;
                    pieces[i].z = *z;
                }
            }
            EffectManager_SpawnParticlesAt(count, 0, ctx->particleStyle, pos, pieces, 300, 120);
        }
    }
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = ((struct SceneActorBehaviorView *)actor)->script1F8;
    if (script) {
        s32 values[8];
        struct ScriptUtilityArguments arguments;
        Ghost_SetScriptValue(actor->body.position.x, &values[0]);
        Ghost_SetScriptValue(actor->body.position.y, &values[1]);
        Ghost_SetScriptValue(actor->body.position.z, &values[2]);
        Ghost_SetScriptValue(((struct SceneActorBehaviorView *)actor)->parameters1FC[0],
                             &values[3]);
        Ghost_SetScriptValue(((struct SceneActorBehaviorView *)actor)->parameters1FC[1],
                             &values[4]);
        Ghost_SetScriptValue(((struct SceneActorBehaviorView *)actor)->parameters1FC[2],
                             &values[5]);
        Ghost_SetScriptValue(actor->field2E0, &values[6]);
        Ghost_SetScriptValue(actor->body.id, &values[7]);
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        u16 *flags;
        u32 zero;
        u32 one;
        u16 masked;
        u32 test = actor->flags2D4 & mask;
        flags = &actor->flags2D4;
        if ((!test) && (((u32)(actor->category - 11)) > 2))
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        {
            u32 mask2 = 0x4000;
            u32 value = *flags;
            masked = value & mask2;
        }
        if (!masked) {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            one = (actor->field2B2 = 1);
            zero = 0;
            actor->field2B0 = masked;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = masked;
            {
                u32 *field = &actor->flags114;
                s32 clear = ~1;
                *field &= clear;
            }
            actor->word104 = masked;
            {
                u32 behavior = 2;
                actor->state2A8 = one;
                actor->phase2A9 = zero;
                actor->behavior2AA = behavior;
                actor->counter2AC = masked;
                actor->field2B1 = one;
            }
        }
    }
}

static inline s32 Ghost_TruncateShift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline void Ghost_MakeRouteMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Ghost_TruncateShift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Ghost_TruncateShift(gUnk_086149C4[angle & 255] * speed, 12);
}

void TentacleActor_ResetOscillation(void *);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

void TentacleActor_SelectWanderRoute(void *);

void TentacleActor_AdvanceMotionPhase(void *, s32);

void sub_080A9DB0(void *, s32, s32);

void TentacleActor_UpdateOscillation(void *, s32, s32);

static inline u8 Ghost_ApproachCurvePosition(struct SceneActorRegistryResetView *actor,
                                             struct SceneActorMotionSubject *saved, s32 speed)
{
    struct SceneActorBody *motion = &actor->body;
    s16 dx = saved->position.x - (u16)motion->position.x;
    s16 dz = saved->position.z - (u16)motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        *(u16 *)&motion->motion.x +=
            Ghost_TruncateShift(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        *(u16 *)&motion->motion.z +=
            Ghost_TruncateShift(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

static inline void Ghost_UpdateAnimationSpeed(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(&((struct GhostVisual *)actor->drawable)->sequence.anim, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
}

static inline void Ghost_FillPosition(void *dst, u32 *tmp, u32 value)
{
    *tmp = value;
    CpuSet(tmp, dst, 0x05000002);
}

static inline s32 Ghost_GetCellAndPosition(const struct ContactPosition *position,
                                           struct ContactPosition **saved)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    *saved = (void *)position;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

// confidence=low: follows saved route points and renews a destination after arrival.

void Ghost_FollowRoute(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *body = actor->context;
    s32 speed;
    u32 zero[1];
    struct ContactPosition target;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        Ghost_FillPosition(&body->motion, zero, 0);
        body->curveDirection = 1;
        actor->field2C2 = 64;
    }
    speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
    Ghost_UpdateAnimationSpeed(actor);
    Ghost_UpdateCurveSway(actor);
    if (Ghost_ApproachCurvePosition(actor, &body->curveSubject, speed) &&
        Ghost_AdvanceMovementCurve(actor)) {
        struct SceneActorRouteCursor *route;
        struct ContactPosition *p;
        struct ContactPosition *saved;
        s32 cell, height;
        u32 kind;
        u16 attr;
        Ghost_MakeRouteMotion((void *)(&body->motion), actor->body.direction, speed);
        route = (void *)&body->route;
        if (!Ghost_SelectRandomRouteEntry(route))
            goto done;
        p = &target;
        p->x = route->point->x;
        p->z = route->point->z;
        cell = Ghost_GetCellAndPosition(p, &saved);
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
        Ghost_StartMovementToPosition(actor, saved);
    }
done:
    actor->counter2AC++;
}

// confidence=low: decays saved horizontal route motion before returning to pursuit or rest.

static inline void Ghost_SetDecelerationState(struct SceneActorRegistryResetView *actor, u32 phase,
                                              u32 behavior, u32 zero)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}

void Ghost_DecelerateMovement(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *ctx = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SceneActorChase_SetControl(actor, actor->entryTarget224, 2);
    actor->body.motion.x += ctx->motion.x;
    actor->body.motion.z += ctx->motion.z;
    if (Mod(actor->counter2AC, 4) == 0) {
        ctx->motion.x >>= 1;
        ctx->motion.z >>= 1;
    }
    if (actor->field2B0) {
        u32 mask = 0x100;
        u16 *p = &actor->flags2D4;
        u16 flags;
        {
            u32 raw = *p & mask;
            flags = (u16)raw;
        }
        if (flags)
            Ghost_SetDecelerationState(actor, 2, 1, 0);
        else
            Ghost_SetDecelerationState(actor, 0, 0, flags);
    } else
        actor->counter2AC++;
}

static inline void Ghost_SetRouteState(struct SceneActorRegistryResetView *actor, u8 state,
                                       u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Ghost_ChooseRouteDestination(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        state->flightPhase = 0;
    }
    if ((s32)actor->counter2AC > 60) {
        struct SceneActorRouteCursor *route;

        route = (void *)&state->route;
        Ghost_SelectRandomRouteEntry(route);
        target = &destination;
        target->x = route->point->x;
        target->z = route->point->z;
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            endpoint = target;
            target->y = SceneActorChase_HeightAt(Ghost_TerrainCell(cellX, cellZ), target);
        }

        Ghost_SetMovementEndpoints(actor, endpoint);
        Ghost_AdvanceMovementCurve(actor);
        Ghost_SetRouteState(actor, 0, 0, 2);
        {
            struct ContactPosition *position = &actor->body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        actor->unknown3CD[0] = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(&state->motion);
    }
    actor->counter2AC++;
}

static inline void Ghost_SetRecoilState(struct SceneActorRegistryResetView *actor, u32 state,
                                        u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline s32 Ghost_ScaleRecoilMotion(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void Ghost_GetRouteEndpoint(struct GhostRouteCursor *path,
                                          struct ContactPosition *pos)
{
    struct SceneActorRoutePoint *point = path->point;
    pos->x = point->x;
    pos->z = path->point->z;
    pos->y = SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
}

static inline void Ghost_ClearPosition(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}

// confidence=low: starts a random recoil then returns to the stored terrain endpoint.

void Ghost_RecoilTowardEndpoint(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    struct ContactPosition pos;
    u32 zero;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        u32 angle;
        u32 n;
        SceneActorChase_SetControl(actor, Ghost_UpdateRecoilAnimation, 0x24);
        Ghost_EndPlayerSlowdown(actor);
        gRandomIndex = n = (gRandomIndex + 1) & 1023;
        angle = *(u8 *)&((const u16 *)RANDOM_TABLE_ADDR)[n];
        {

            Ghost_ClearPosition(&context->motion, &zero);
        }
        actor->secondaryMotion.x = Ghost_ScaleRecoilMotion(gUnk_086149C4[angle] * 64);
        actor->secondaryMotion.z = Ghost_ScaleRecoilMotion(gUnk_086149C4[(angle + 64) & 255] * 64);
        actor->body.direction =
            Math_Atan2(actor->secondaryMotion.x, actor->secondaryMotion.z) - 128;
    }
    if (actor->field2B0) {
        Ghost_ClearActionFlags(actor);
        Ghost_ClearPosition(&actor->secondaryMotion, &zero);
        Ghost_SetRecoilState(actor, 0, 0, 2);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        {
            struct GhostRouteCursor *path = &context->route;
            Ghost_SelectRandomRouteEntry(path);
            Ghost_GetRouteEndpoint(path, &pos);
        }
        Ghost_StartMovementToPosition(actor, &pos);
    } else
        ++actor->counter2AC;
}

static inline s32 Ghost_ScaleFixed12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void Ghost_MakePolarMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Ghost_ScaleFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Ghost_ScaleFixed12(gUnk_086149C4[angle & 255] * speed);
}

static inline void Ghost_SubtractPosition(struct ContactPosition *out,
                                          const struct ContactPosition *from,
                                          const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline u16 Ghost_NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + RANDOM_TABLE_ADDR);
}

static inline u16 Ghost_NextMotionRandom(void)
{
    u32 offset;
    const u16 *table;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    return *(const u16 *)(offset + (u32)table);
}

static inline u32 Ghost_OffsetHeading(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}

static inline void Ghost_SetState(struct SceneActorRegistryResetView *actor, u8 phase, u8 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline s32 Ghost_QuarterDistance(s32 n)
{
    if (n >= 0)
        return n >> 2;
    return -(-n >> 2);
}

static inline void Ghost_SetColliderFlags(struct ContactCollider *c, u32 bits)
{
    c->flags |= bits;
}

inline static void Ghost_DisableCollider(struct ContactCollider *c)
{
    {
        u8 mask = 4;
        c->flags &= ~mask;
    }
}

static inline u8 Ghost_TestRouteFlags(struct Ghost *route, u32 mask)
{
    u32 flags = route->flags;
    if (flags & mask)
        return 1;
    return 0;
}

static inline void Ghost_ClearMotionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u16 *flags = &actor->flags2D4;
    u32 mask = ~bits;
    *flags &= mask;
}

static inline void Ghost_ClearRouteFlags(struct Ghost *route, u32 bits)
{
    u32 mask = ~bits;
    route->flags &= mask;
}

static inline s32 Ghost_LengthSquared(const struct ContactPosition *position)
{
    return position->x * position->x + position->z * position->z;
}

static inline void Ghost_AddPosition(struct ContactPosition *out,
                                     const struct ContactPosition *from)
{
    out->x = out->x + from->x;
    GHOST_SET_VERTICAL(out, out->y + from->y);
    out->z = out->z + from->z;
}

static inline void Ghost_MakeRetreatOffset(struct ContactPosition *out, u32 angle)
{
    s32 x, z;
    s32 speed = 728;
    u32 zero;
    x = Ghost_ScaleFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    zero = 0;
    GHOST_WRITE_COMPONENT(out, x, x);
    GHOST_SET_VERTICAL(out, zero);
    z = Ghost_ScaleFixed12(gUnk_086149C4[angle & 255] * speed);
    GHOST_WRITE_COMPONENT(out, z, z);
}

// confidence=low: follows a finite curved route toward or away from the player.

void Ghost_AdvanceApproachRoute(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *route = actor->context;
    struct ContactPosition target;
    struct ContactPosition delta;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        route->index = 0;
        if (Ghost_TestRouteFlags(route, 4)) {
            u8 reverseBias = 128;
            Ghost_MakeRetreatOffset(
                &target,
                (actor->tracking.heading + ((Ghost_NextRandom() & 63) + reverseBias)) & 255);
            Ghost_AddPosition(&target, &actor->body.position);
        } else {
            target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        }
        Ghost_SubtractPosition(&delta, &actor->body.position, &target);
        route->approachDistance = Ghost_QuarterDistance(Ghost_LengthSquared(&delta));
        Ghost_MakePolarMotion(
            &route->motion,
            Ghost_OffsetHeading(Ghost_NextMotionRandom(), &actor->tracking.heading) & 255, 32);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed = 1;
        {
            u32 mask = 0x100;
            actor->flags2D4 = mask | actor->flags2D4;
        }
        actor->field2C2 = 32;
    }
    Ghost_SubtractPosition(&delta, &actor->body.position, &route->points[route->index]);
    if (!Ghost_TestRouteFlags(route, 4))
        Ghost_DisableCollider(&actor->colliderB);
    Ghost_EmitAfterimage(actor);
    if ((Ghost_LengthSquared(&delta)) > 1024) {
        actor->body.direction = Math_Atan2(route->points[route->index].x - actor->body.position.x,
                                           route->points[route->index].z - actor->body.position.z);
        Ghost_MakePolarMotion(&delta, actor->body.direction, route->routeSpeed);
        actor->body.motion.x += delta.x;
        actor->body.motion.z += delta.z;
        route->motion = actor->body.motion;
    } else if ((++route->index) > 8) {
        actor->body.position.x = route->points[8].x;
        actor->body.position.z = route->points[8].z;
        Ghost_ClearMotionFlags(actor, 0x100);
        Ghost_SetColliderFlags(&actor->colliderB, 4);
        if (Ghost_TestRouteFlags(route, 4)) {
            Ghost_SetState(actor, 0, 0);
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
            Ghost_ClearRouteFlags(route, 4);
        } else {
            Ghost_SetState(actor, 2, 1);
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        }
        return;
    }
    if (actor->paletteOffset == 0) {
        if ((Mod(actor->counter2AC, 8) == 0) && (route->routeSpeed <= 19))
            ++route->routeSpeed;
    } else if ((Mod(actor->counter2AC, 4) == 0) && (route->routeSpeed <= 19))
        route->routeSpeed += 2;
}

extern u8 gUnk_08605DD4[];

void Ghost_InitMotionPreset(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = gUnk_08605DD4;
}

void SceneActor_BeginLinkedIdleReaction(struct SceneActorRegistryResetView *actor)
{
    if (!SceneActor_ActionsBlocked(actor) && Ghost_HasPlayerLink(actor->context)) {
        u32 three = 3;
        u8 *state = &actor->state2A8;
        u8 zero = 0;
        u8 one;
        void (*fn)(void);
        u8 code;
        *state = zero;
        actor->phase2A9 = zero;
        actor->behavior2AA = three;
        actor->counter2AC = zero;
        {
            u8 *queued = &actor->field2B1;
            one = 1;
            *queued = one;
        }
        fn = SceneActorControl_OnIdle;
        code = 0x10;
        actor->field2B2 = one;
        actor->field2B0 = zero;
        actor->field2B5 = code;
        actor->target = fn;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
        actor->colliderA.flags &= 0xFFFB;
    }
}

/* actor must be nonnull: its control context is loaded before the availability gate. */

void Ghost_RequestVisibility(struct SceneActorRegistryResetView *input, const u16 *inputRequest)
{
    struct SceneActorRegistryResetView *actor = input;
    const u16 *request = inputRequest;
    struct Ghost *state = actor->context;
    u32 unavailable = SceneActor_ActionsBlocked(actor);
    if (unavailable)
        return;
    {
        u32 mask = 0x4000;
        u16 *flags;
        flags = &actor->flags2D4;
        if (!(*flags & mask))
            return;
    }
    {
        u32 requested = request[1];
        if (requested == 0) {
            u32 mask = 0x20;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = &state->flags;
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (hidden) {
                    {

                        u16 *timer;
                        timer = &state->visibilityTimer;
                        *timer = 0;
                    }
                    {
                        u32 mask = ~0x240;
                        {
                            u32 old = *savedFlags & mask;
                            u32 bits = 0x20;
                            *savedFlags = old | bits;
                        }
                    }
                    Sound_PlaySong(0x21E);
                    {
                        u32 mask = 0x800;
                        u16 *flags;
                        flags = &actor->flags2C0;
                        mask |= *flags;
                        *flags = mask;
                    }
                }
            }
        } else {
            u32 mask = 0x40;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = &state->flags;
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (!hidden) {
                    u32 hiddenZero = hidden;
                    {
                        u32 zero = hiddenZero;

                        u16 *timer;
                        timer = &state->visibilityTimer;
                        *timer = zero;
                    }
                    {
                        u32 mask = ~0x220;
                        {
                            u32 old = *savedFlags & mask;
                            u32 bits = 0x40;
                            *savedFlags = old | bits;
                        }
                    }
                    Sound_PlaySong(0x21E);
                    {
                        u32 mask = 0x800;
                        u16 *flags;
                        flags = &actor->flags2C0;
                        mask |= *flags;
                        *flags = mask;
                    }
                }
            }
        }
    }
}

#define gSceneActorSineTable gUnk_086149C4

static inline void Ghost_EaseHeight(s32 target, s16 *height)
{
    *height += Div(target - *height, 10);
}

static inline s32 Ghost_ScaleHeightWave(s32 wave)
{
    if (wave >= 0)
        return wave >> 12;
    return -(-wave >> 12);
}

void SceneActor_EasePlayerHeight(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    u32 active = gPlayerPassageState;
    s32 special = 0;
    s32 offset;
    s32 target;
    if (active && gPlayerInteractionMode == 5)
        special = 1;
    if (special || gPlayers[0]->actionMode == 4)
        offset = 100;
    else
        offset = 220;
    if (actor->field2B5 == 0x23) {
        target = gPlayers[0]->position.y + offset;
        Ghost_EaseHeight(target, &actor->body.position.y);
    } else {
        s32 wave = gUnk_086149C4[state->flightPhase] << 5;
        target = gPlayers[0]->position.y + offset + Ghost_ScaleHeightWave(wave);
        Ghost_EaseHeight(target, &actor->body.position.y);
        state->flightPhase += 2;
    }
}

void Ghost_UpdateCurveSway(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    s16 *amplitude = &context->curve;
    s32 value = *amplitude;
    if (value > 0x140) {
        context->curveDirection = 0xff;
        *amplitude = 0x140;
    } else if (value < -0x140) {
        context->curveDirection = 1;
        *amplitude = -0x140;
    }
    *(u16 *)&context->curve += context->curveDirection * 12;
}

u8 *Path_GetPointData(u8 *);

static inline u32 Ghost_NextRouteRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gRandomTable);
}

u8 Ghost_SelectRandomRouteEntry(struct SceneActorRouteCursor *cursor)
{
    struct SceneActorRouteCursor *result = cursor;
    u8 pointIndex;
    if (cursor->definition->pointCount <= 1)
        return 0;
    pointIndex = Mod(Ghost_NextRouteRandom(), cursor->definition->pointCount);
    if (pointIndex == cursor->pointIndex)
        pointIndex = Mod(pointIndex + 1, cursor->definition->pointCount);
    result->pointIndex = pointIndex;
    result->unknown3 = 0;
    result->point = (void *)(Path_GetPointData((void *)result->definition) +
                             result->pointIndex * sizeof(*result->point));
    return 1;
}

struct SceneActorRegistryActor;

void SceneActorCollision_InitBody(struct SceneActorRegistryActor *, void *, void *, void *, void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void SceneActor_InitLinkedReactionColliders(struct SceneActorRegistryResetView *actor)
{
    union GhostColliderVector size, offset;
    size.packed.x = 16;
    size.packed.y = 64;
    size.packed.z = 16;
    offset.packed.x = 0;
    offset.packed.y = 0;
    offset.packed.z = 0;
    SceneActorCollision_InitBody((void *)actor, &size, &offset, SceneActorBehavior_OnContactForward,
                                 actor);
    size.position.x = 24;
    size.position.y = 64;
    size.position.z = 24;
    offset.position.x = 0;
    offset.position.y = 0;
    offset.position.z = 0;
    {
        u32 flags = 0x2003;
        struct ContactCollider *collider = &actor->colliderB;
        Collider_Init(collider, actor->body.id, flags, 0, 16, &size, &offset);
        Collider_SetAttackProperties(&actor->colliderB, 0, 0, 0, 0, 0);
        Collider_SetPosition(&actor->colliderB, &actor->body.position, 0);
        Collider_SetCallback(&actor->colliderB, Ghost_BeginContactReaction, actor);
    }
}

s32 Ghost_GetBehaviorCode(void)
{
    return 4;
}

void Ghost_SetMovementEndpoints(struct SceneActorRegistryResetView *actor,
                                struct ContactPosition *destination)
{
    struct Ghost *context = actor->context;
    context->start = actor->body.position;
    context->end = *destination;
    context->curveTime = 0;
}

u8 Ghost_AdvanceMovementCurve(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *ctx = actor->context;
    struct ContactPosition start, end;
    start = ctx->start;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
    }
    end = ctx->end;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&end = mask & *(u32 *)&end;
    }
    MovementCurve_InterpolatePosition(&ctx->curveSubject.position, &start, &end, 1, ctx->curveTime,
                                      0, ctx->curve);
    if (ctx->curveTime > 1023)
        return 1;
    ctx->curveTime += 128;
    return 0;
}

void Ghost_InitActorProfile(struct SceneActorRegistryResetView *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(0x17, 0);
}

static inline void Ghost_FillMovementPosition(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

void Ghost_ResetMovementState(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    u32 tmp;
    u32 one;
    context->curveTime = 0;
    context->flightPhase = 0;
    {
        u8 *active = &context->curveDirection;
        one = 1;
        *active = one;
    }
    context->curve = 0;
    context->drainElapsed = 0;
    context->drainStart = 0;
    context->flags = actor->saved.privateFlags;
    *(struct GhostPackedPosition *)&context->previousPlayer =
        *(struct GhostPackedPosition *)&gPlayers[0]->position;
    Ghost_FillMovementPosition(&context->start, &tmp, 0);
    Ghost_FillMovementPosition(&context->end, &tmp, 0);
    Ghost_FillMovementPosition(&context->motion, &tmp, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

static inline u8 Ghost_TestLinkedFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

void SceneActor_EndLinkedReaction(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
    }
    {
        u32 *flags = &state->flags;
        u32 mask = ~0x11;
        *flags &= mask;
    }
    {
        struct ContactCollider *collider = &actor->colliderA;
        collider->flags &= 0xFFFB;
    }
    {
        u32 mask = 0x80;
        if (Ghost_TestLinkedFlags(state->flags, mask)) {
            u8 *count = &gPlayers[0]->movementSpeedReduction;
            *count -= 3;
            {
                u32 *flags = &state->flags;
                u32 mask = ~0x80;
                *flags &= mask;
            }
        }
    }
}

void Ghost_GetDirectionalFrame(u16 heading, s32 *variant, s32 *flags)
{
    s32 t = ((heading + 0x70) >> 5) & 7;
    if (t > 4) {
        *variant = 8 - t;
        *flags = 1;
    } else {
        *variant = t;
        *flags = 0;
    }
}

void Ghost_StartMovementToPosition(struct SceneActorRegistryResetView *actor, s16 *destination)
{
    struct Ghost *context = actor->context;
    u32 workspace[3];
    s16 *position;
    s16 *delta;
    Ghost_SetMovementEndpoints(actor, destination);
    Ghost_AdvanceMovementCurve(actor);
    {
        u32 one = 1;
        u8 *state = &actor->state2A8;
        u32 zero = 0;
        *state = zero;
        {
            actor->phase2A9 = zero;
            actor->behavior2AA = one;
            actor->counter2AC = zero;
        }
        actor->field2B1 = one;
        delta = (s16 *)(workspace + 1);
        position = (s16 *)&actor->body.position;
        delta[0] = destination[0] - position[0];
        delta[1] = destination[1] - position[1];
        delta[2] = destination[2] - position[2];
        actor->unknown3CD[0] = Math_Atan2(delta[0], delta[2]);
        Ghost_FillMovementPosition(&context->motion, workspace, zero);
    }
}

void Ghost_ClearActionFlags(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    u16 *motionFlags = &actor->flags2D4;
    u32 motionMask = 0xFFFFFEFF;
    u32 *actionFlags;
    u32 actionMask;
    struct SceneActorChaseTracking *tracking;
    u32 trackingMask;
    u16 zero;
    u16 motionValue;
    motionValue = *motionFlags & motionMask;
    zero = 0;
    *motionFlags = motionValue;
    tracking = &actor->tracking;
    trackingMask = ~8;
    tracking->flags = tracking->flags & trackingMask;
    actionFlags = &((struct Ghost *)actor->context)->flags;
    actionMask = ~0x10;
    *actionFlags = *actionFlags & actionMask;
    context->delay = zero;
}

// confidence=low: when the idle flag is set and the delay expires, picks a random route point and starts moving to it.

static inline u16 Ghost_TestDestinationFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    mask &= actor->flags2D4;
    return mask;
}

static inline u8 Ghost_HasDestinationFlags(struct Ghost *c, u32 mask)
{
    if (c->flags & mask)
        return 1;
    return 0;
}

void Ghost_UpdateRandomDestination(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (Ghost_TestDestinationFlags(actor, 0x120))
        return;
    {
        u8 ready = Ghost_HasDestinationFlags(state, 0x10);
        if (!ready) {
            state->delay = ready;
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
            u32 zero, two;
            Ghost_SelectRandomRouteEntry(route);
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
            zero = 0;
            Ghost_StartMovementToPosition(actor, &target);
            Ghost_ClearActionFlags(actor);
            SceneActor_EndLinkedReaction(actor);
            two = 2;
            actor->state2A8 = zero;
            actor->phase2A9 = zero;
            actor->behavior2AA = two;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
    }
}

static inline u32 Ghost_ReadReactionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline u8 Ghost_TestReactionFlags(struct SceneActorReactionMotion *reaction, u32 mask)
{
    if (reaction->flags & mask)
        return 1;
    return 0;
}

static inline u8 Ghost_GetReactionFlags(struct Ghost *state, u32 input, u32 **saved)
{
    u32 mask = input;
    u32 *flags = &state->flags;
    u32 tested;
    tested = *flags & mask;
    *saved = flags;
    if (tested)
        return 1;
    return 0;
}

static inline void Ghost_FindPositionDelta(struct ContactPosition *out,
                                           const struct ContactPosition *from,
                                           const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline void Ghost_SetPlayerReactionState(struct SceneActorRegistryResetView *actor, u8 state,
                                                u8 phase, u32 behavior, u32 counter)
{
    u32 selected = behavior;
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = selected;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}

// confidence=low: reacts to player motion within range, with separate drain and approach transitions.

static inline void Ghost_SetIdleBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    *field = zero;
    actor->phase2A9 = zero;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}

void Ghost_ReactToPlayerMovement(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state;
    struct ContactPosition delta;
    s32 height;
    u32 active;
    u32 *savedFlags;
    struct SceneActorReactionMotion *reaction;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (Ghost_ReadReactionFlags(actor, 0x20))
        return;
    state = actor->context;
    {
        u8 **players = &(*(u8 **)gPlayers);
        u8 **slots;
        slots = players;
        Ghost_FindPositionDelta(&delta, &state->previousPlayer, (void *)&gPlayers[0]->position);
    }
    state->previousPlayer = *(struct ContactPosition *)&gPlayers[0]->position;
    if (state->reactionDelay) {
        state->reactionDelay--;
        return;
    }
    if (Ghost_ReadReactionFlags(actor, 0x100))
        return;
    if (!delta.x && !delta.z)
        return;
    reaction = (void *)&actor->tracking.target;
    if (!Ghost_TestReactionFlags(reaction, 2))
        return;
    {
        u32 mask = ~2;
        reaction->flags = mask & reaction->flags;
    }
    height = actor->body.position.y - gPlayers[0]->position.y;
    if (height < 0)
        height = -height;
    if (height > 512)
        return;
    if (actor->tracking.distance >= actor->tracking.limit)
        return;
    active = Ghost_GetReactionFlags(state, 0x10, &savedFlags);
    if (active) {
        if (gPlayers[0]->currentStatB == 0)
            return;
        Ghost_SetIdleBehavior(actor, 4);
        state->delay = state->delayDuration;
    } else {
        {
            u32 mask = 0x10;
            *savedFlags |= mask;
        }
        Ghost_SetPlayerReactionState(actor, active, 2, 1, active);
        state->delay = state->delayDuration;
        state->reactionDelay = 10;
    }
}

u32 Ghost_GetControlStatus(void)
{
    return 0;
}

void Ghost_OnControlCleanup(void)
{
}

/* Tentative palette resource word shared by all four afterimages. */

void Ghost_SetAfterimagePaletteWord(struct SceneActorRegistryResetView *actor, u32 value)
{
    struct Ghost *g = actor->context;
    s32 i;
    for (i = 3; i >= 0; i--)
        g->visuals[i].sequence.animation.paletteWord = value;
}

void SceneActor_InitLinkedSprites(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    struct GhostVisualSequence *animation = &state->visuals[0].sequence;
    struct ActorSprite *sprite = &state->visuals[0].sprite;
    s32 i;
    /* agbcc retains independent sprite and sequence cursors in the original. */
    for (i = 0; i < 4; i++) {
        void *script;
        AnimData_Load(&animation->animation, 0xD637);
        Sprite_Init(sprite, &animation->animation, 0);
        AnimData_SelectPaletteBank(&animation->animation, 0x1EE);
        script = ScriptProc_Start(0x922E, 0x29FB);
        state->visuals[i].sequence.resource = script;
        sprite->flags = 0x201;
        animation->life = 0;
        animation = (void *)((u8 *)animation + sizeof(struct GhostVisual));
        sprite = (void *)((u8 *)sprite + sizeof(struct GhostVisual));
    }
    state->visualIndex = 0;
    state->visualInterval = 2;
}

static inline u32 Ghost_GetFrameFactor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}

// confidence=low: refreshes one of four timed Ghost afterimage sprites at the actor's position.

inline static void Ghost_StepAfterimage(struct ActorSprite *o, struct ActorAnim *a,
                                        struct GhostVisual *visual)
{
    const u16 *entry;
    u8 done;
    entry = a->data + a->frame;
    o->frame = (*entry) >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != ((((*entry) & 0x30) >> 4) & bit))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != ((((*entry) & 0x30) >> 4) & bit))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    if (++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else if ((++a->frame) >= a->count) {
            if ((u8)(a->flags & 8)) {
                a->timer = a->rate;
                a->frame = a->count - 1;
                done = 1;
            } else {
                a->frame = 0;
                done = 1;
            }
        } else
            done = 0;
        entry = a->data + a->frame;
        a->factor = Ghost_GetFrameFactor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
}

void Ghost_EmitAfterimage(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *ctx = actor->context;
    u32 variant;
    u32 flags;
    if (Mod(actor->field2CA, ctx->visualInterval) == 0) {
        Ghost_GetDirectionalFrame(actor->body.direction, &variant, &flags);
        {
            struct ActorAnim *source = &((struct GhostVisual *)actor->drawable)->sequence.anim;
            u32 sequence = source->unused;
            {
                Anim_SetSequence(&ctx->visuals[ctx->visualIndex].sequence.anim,
                                 ctx->visuals[ctx->visualIndex].sequence.resource, sequence,
                                 (u8)variant, (u8)flags);
            }
        }
        Ghost_StepAfterimage(&(&ctx->visuals[ctx->visualIndex])->sprite,
                             &ctx->visuals[ctx->visualIndex].sequence.anim,
                             &ctx->visuals[ctx->visualIndex]);
        {
            u32 offset = ctx->visualIndex * sizeof(struct GhostVisual);
            struct ContactPosition *base = (void *)&ctx->visuals[0].sprite.x;
            struct ContactPosition *position = (void *)((u8 *)base + offset);
            *position = actor->body.position;
        }
        ctx->visuals[ctx->visualIndex].sprite.flags &= ~1;
        ctx->visuals[ctx->visualIndex].sequence.life = 8;
        if ((++ctx->visualIndex) > 3)
            ctx->visualIndex = 0;
    }
}

void ActorContact_TickAuxiliaryTimers(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    struct ActorSprite *sprite = &context->visuals[0].sprite;
    struct Ghost *entry = context;
    u32 lifeOffset = (u32) & ((struct Ghost *)0)->visuals[0].sequence.life;
    s32 i;
    /* Keep the life cursor independent of the sprite cursor for agbcc. */
    i = 3;
loop: {
    u16 *timer = (void *)((u8 *)entry + lifeOffset);
    if (*timer) {
        --*timer;
        if (*timer == 0)
            sprite->flags |= 1;
    }
    sprite = (void *)((u8 *)sprite + sizeof(struct GhostVisual));
    entry = (void *)((u8 *)entry + sizeof(struct GhostVisual));
}
    if (--i >= 0)
        goto loop;
}

void Ghost_RemoveAuxiliarySprites(struct Ghost *context)
{
    struct GhostVisual *visual = context->visuals;
    s32 remaining = 3;
    do {
        Sprite_Remove(&visual->sprite);
        visual++;
        remaining--;
    } while (remaining >= 0);
}

static inline s32 Ghost_Shift10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}

static inline void Ghost_SubtractPoints(struct ContactPosition *out, struct ContactPosition *a,
                                        struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void Ghost_InterpolatePoint(struct ContactPosition *out,
                                          struct ContactPosition *start,
                                          struct ContactPosition *end, s32 tx, s32 tz, s32 t)
{
    struct ContactPosition offset;
    struct ContactPosition acc;
    s32 q, p, twiceP, thriceQ, tangent, h11;
    s32 scaled, value;
    Ghost_SubtractPoints(&offset, end, start);
    ZeroFill8(&acc);
    q = Ghost_Shift10(t * t);
    p = Ghost_Shift10(q * t);
    twiceP = p * 2;
    thriceQ = q * 3;
    tangent = (p - 2 * q + t) * tz;
    h11 = p - q;
    scaled = ((twiceP - thriceQ + 1) * acc.x) * tx;
    value = (thriceQ - twiceP) * offset.x;
    value = scaled + value + tangent + h11;
    out->x = start->x + Ghost_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.y) * tx;
    value = (-twiceP + thriceQ) * offset.y;
    value = scaled + value + tangent + h11;
    out->y = start->y + Ghost_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.z) * tx;
    value = (-twiceP + thriceQ) * offset.z;
    value = scaled + value + tangent + h11;
    out->z = start->z + Ghost_Shift10(value);
}

void Ghost_BuildMovementCurve(struct SceneActorRegistryResetView *actor,
                              struct ContactPosition *target)
{
    struct Ghost *route = actor->context;
    struct ContactPosition start;
    struct ContactPosition end;
    s32 step, tx, tz, i;

    start = actor->body.position;
    end = *target;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
    }
    *(u32 *)&end &= 0xFFFF;
    step = Div(0x400, 8);
    tx = route->motion.x << 7;
    tz = route->motion.z << 7;
    for (i = 0; i < 8; i++) {
        s32 next = i + 1;
        struct ContactPosition *out =
            (struct ContactPosition *)(i * sizeof(route->points[0]) + (u32)route);
        out = ((struct Ghost *)out)->points;
        Ghost_InterpolatePoint(out, &start, &end, tx, tz, step * next);
    }
    route->points[8] = end;
}

static inline void Hide(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    {
        u32 *flags = actor->drawable;
        u32 value = *flags;
        u32 one = 1;
        *flags = value | one;
    }
    {

        ActorShadow_Disable(actor->shadow);
    }
}

static inline void Show(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    {
        u32 *flags = actor->drawable;
        u32 value = *flags;
        u32 mask;
        mask = ~1;
        *flags = value & mask;
    }
    {

        ActorShadow_Enable(actor->shadow);
    }
}

// confidence=low: flickers the drawable and shadow during a pending visibility transition.

void Ghost_TickVisibilityTransition(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct Ghost *state;
    u32 unavailable = SceneActor_ActionsBlocked(actor);
    if (unavailable)
        return;
    {

        u32 mask;
        u32 active;
        state = actor->context;
        mask = 0x60;
        active = SceneActorControl_TestFlags(state->flags, mask);
        if (!active)
            return;
    }
    {
        /* The engine flags are tested here, but the result goes unused. */
        u8 ignored = SceneActorControl_TestFlags(gEngineFlags, 0x4800);
        u32 counter;
        u32 value;
        {

            u16 *field;
            field = &state->visibilityTimer;
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            if ((u8)(counter >> 18) & 1)
                Show(actor);
            else
                Hide(actor);
        } else if (value <= 29) {
            if ((u8)(counter >> 17) & 1)
                Show(actor);
            else
                Hide(actor);
        } else {
            u32 mask = 0x20;
            u32 *inputFlags;
            u32 *flags;
            u32 active;
            inputFlags = &state->flags;
            active = *inputFlags & mask;
            flags = inputFlags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (active) {
                {
                    u32 *field = actor->drawable;
                    u32 value = *field;
                    u32 mask = ~1;
                    *field = value & mask;
                }
                {
                    u32 mask = ~0x20;
                    *flags &= mask;
                }
                ActorShadow_Enable(actor->shadow);
            } else {
                {
                    u32 *field = actor->drawable;
                    u32 value = *field;
                    u32 one = 1;
                    *field = value | one;
                }
                {
                    u32 mask = ~0x40;
                    *flags &= mask;
                }
                ActorShadow_Disable(actor->shadow);
            }
        }
    }
    {
        u16 *counter = &state->visibilityTimer;
        (*counter)++;
    }
}

static inline u8 Ghost_TestRevealFlags(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}

void ActorVisibility_TickRevealDelay(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *s = actor->context;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (Ghost_TestRevealFlags(gEngineFlags, 0x4800)) {
        DEBUG_PRINT();
    }
    if (s->revealTicks) {
        *(u32 *)actor->drawable &= ~1;
        s->revealTicks--;
    }
}

static inline u8 Ghost_TestSlowdownFlags(u32 flags, u32 mask)
{
    if (flags & mask)
        return TRUE;
    return FALSE;
}

void Ghost_EndPlayerSlowdown(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *q = actor->context;
    u32 mask = 0x80;

    if (Ghost_TestSlowdownFlags(q->flags, mask)) {
        u8 *p = &gPlayers[0]->movementSpeedReduction;
        *p = *p - 3;
        {
            u32 off2 = 0x558;
            u32 *f = &((struct Ghost *)actor->context)->flags;
            u32 m = ~0x80;
            *f = *f & m;
        }
    }
}

void ActorTick_DecrementCounterAA(struct SceneActorRegistryResetView *actor)
{
    if (!SceneActor_ActionsBlocked(actor)) {
        s16 *p = (s16 *)&actor->fieldAA;
        s32 v = *p;
        if (v > 0)
            *p = v - 1;
    }
}

static inline u8 Ghost_TestVisibilityFlags(struct Ghost *p, u32 mask)
{
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline u32 Ghost_ReadMotionFlags(struct SceneActorRegistryResetView *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u16 *flags = &actor->flags2D4;
    return *flags & mask;
}

static inline u8 Ghost_IsLocalPlayerRevealing(void)
{
    struct Player *p = gPlayers[0];
    if (p) {
        u32 state = p->active;
        u32 result = 0;
        if (state && p->sceneMode == 4)
            result = 1;
        return result;
    }
    return 0;
}

static inline u8 Ghost_IsGlobalRevealActive(void)
{
    u32 state = gPlayerPassageState;
    u32 result = 0;
    if (state && gPlayerInteractionMode == 4)
        result = 1;
    return result;
}

static inline u8 Ghost_TestSaveFlags(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}

static inline u32 Ghost_ReadPlayerRevealFlag(struct Player *p)
{
    u32 *flags = &p->actionFlags;
    u32 mask = 0x10;
    return *flags & mask;
}

static inline u32 Ghost_TestGlobalFlags(u32 flags, u32 mask)
{
    return flags & mask ? 1 : 0;
}

static inline void Ghost_ClearVisibilityFlags(struct Ghost *context, u32 bits)
{
    u32 mask = ~bits;
    context->flags &= mask;
}

static inline void Ghost_SetVisibilityFlags(struct Ghost *context, u32 bits)
{
    context->flags |= bits;
}

u32 Ghost_UpdateVisibility(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *saved = actor;
    struct Ghost *ctx;
    {
        struct ContactCollider *c = &actor->colliderB;
        c->flags |= 4;
    }
    if (Ghost_ReadMotionFlags(actor, 0x4000)) {
        if (Ghost_TestVisibilityFlags((void *)actor, 0x200)) {
            *((u32 *)actor->drawable) |= 1;
            ActorShadow_Disable(actor->shadow);
            {
                s8 mask = ~0x20;
                u32 *flags = &((struct Ghost *)saved)->flags;
                *flags &= mask;
            }
        } else
            Ghost_TickVisibilityTransition(actor);
        ActorTick_DecrementCounterAA(actor);
        goto finish;
    }
    ctx = actor->context;
    if (!SceneActor_ActionsBlocked(actor)) {
        s32 reveal;
        Ghost_TestGlobalFlags(gEngineFlags, 0x4800);
        if (Ghost_TestVisibilityFlags(ctx, 0x100)) {
            reveal = Ghost_TestSaveFlags(0x20);
        } else
            reveal = gGameState->unk876;
        if (reveal > 0) {
            u32 mode;
            if (gEngineFlags & 0x800)
                mode = Ghost_IsLocalPlayerRevealing();
            else {
                if (gWeatherCollisionTimer > 0)
                    goto hide;
                if (gPlayers[0] && Ghost_ReadPlayerRevealFlag(gPlayers[0]))
                    goto hide;
                mode = Ghost_IsGlobalRevealActive();
            }
            if (mode)
                goto hide;
            if (!Ghost_TestVisibilityFlags(ctx, 0x40)) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (!hidden) {
                    ctx->visibilityTimer = hidden;
                    Ghost_ClearVisibilityFlags(ctx, 0x20);
                    Ghost_SetVisibilityFlags(ctx, 0x40);
                }
            }
        } else {
        hide:
            if (!Ghost_TestVisibilityFlags(ctx, 0x20)) {
                if ((u8)SceneActorVisibility_IsHidden(actor)) {
                    ctx->visibilityTimer = 0;
                    Ghost_ClearVisibilityFlags(ctx, 0x40);
                    Ghost_SetVisibilityFlags(ctx, 0x20);
                }
            }
        }
    }
    Ghost_TickVisibilityTransition(actor);
    ActorVisibility_TickRevealDelay(actor);
    Ghost_UpdateRandomDestination(actor);
    Ghost_ReactToPlayerMovement(actor);
finish:
    SceneActor_EasePlayerHeight(actor);

    return 1;
}

u8 SceneActorBehavior_IsNearParticipant(struct SceneActorRegistryActor *);

static inline u32 Ghost_ReadColliderControl(u32 mask, u16 *control)
{
    return *control & mask;
}

u32 ActorContact_UpdateColliderPositions(struct SceneActorBehaviorView *actor)
{
    u16 *control;
    struct ContactPosition *position;
    ActorContact_TickAuxiliaryTimers(actor);
    {
        u32 mask = 0x20;

        control = &actor->flags2D4;
        if (*control & mask)
            goto done;
    }
    if (Ghost_ReadColliderControl(0x110, control))
        goto secondary;
    {
        u32 near = (u32)SceneActorBehavior_IsNearParticipant((void *)actor) << 24;
        position = (void *)actor;
        position = &((struct SceneActorBehaviorView *)position)->position;
        if (!near || Ghost_ReadColliderControl(6, control))
            goto primary;
    }
secondary: {
    struct ContactCollider *collider = &actor->secondaryCollider;
    u32 flags = collider->flags;
    u32 disabled = 4;
    disabled &= flags;
    position = (void *)actor;
    position = &((struct SceneActorBehaviorView *)position)->position;
    if (!disabled) {
        Collider_SetPosition(collider, position, 0);
        Collider_Refresh(collider);
    }
}
primary:
    Collider_SetPosition(&actor->primaryCollider, position, 0);
done:
    return 1;
}

static inline u8 Ghost_TakeStateEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline void Ghost_UpdateAnimationScale(struct SceneActorRegistryResetView *actor, u16 *flags)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (*flags & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(&((struct GhostVisual *)actor->drawable)->sequence.anim, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
}

static inline void Ghost_SubtractPositions(struct ContactPosition *out,
                                           const struct ContactPosition *from,
                                           const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline void Ghost_SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void Ghost_SavePlayerPosition(struct ContactPosition *out, struct Player **players)
{
    struct Player **source = players;
    *out = *(struct ContactPosition *)&(*source)->position;
}

/* Follow the drained player's displacement until the drain finishes. */

static inline void Ghost_ClearDrainCollider(struct ContactCollider *c)
{
    {
        u8 mask = 4;
        c->flags &= ~mask;
    }
}

static inline u16 *Ghost_BeginPlayerFollow(struct SceneActorRegistryResetView *actor,
                                           struct Ghost *context)
{
    u8 taken = Ghost_TakeStateEntry(&actor->field2B1);
    if (taken) {
        SceneActorChase_SetControl(actor, Ghost_UpdateEnergyDrainAnimation, 0x23);
        Ghost_SavePlayerPosition(&context->start, gPlayers);
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = 0x100;
            *flags = mask | (*flags);
        }
        {
            u32 mask = 0x2000000;
            actor->flags2CC |= mask;
        }
        actor->body.direction = actor->tracking.heading;
    }
    return &actor->flags2D4;
}

void Ghost_FollowDrainedPlayer(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    struct ContactPosition delta;
    u16 *flags = Ghost_BeginPlayerFollow(actor, context);
    {
        u32 scale = 64;
        actor->field1EA = scale;
        Ghost_SubtractPositions(&delta, &context->start,
                                (struct ContactPosition *)&gPlayers[0]->position);
        actor->body.motion.x += delta.x;
        actor->body.motion.z += delta.z;
        context->start = *((struct ContactPosition *)&gPlayers[0]->position);
        Ghost_UpdateAnimationScale(actor, flags);
    }
    if (actor->field2B0 || (gPlayers[0]->currentStatB == 0)) {
        Sound_PlaySong(0x3CC);
        Ghost_SetBehavior(actor, 3);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        Ghost_ClearDrainCollider(&actor->colliderA);
    } else {
        ++actor->counter2AC;
    }
}

static inline s32 Ghost_ScaleLoopMotion(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void Ghost_MakeLoopMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Ghost_ScaleLoopMotion(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Ghost_ScaleLoopMotion(gUnk_086149C4[angle & 255] * speed);
}

static inline u32 Ghost_FirstLoopRandom(u32 bits)
{
    u32 index, value;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    index = gRandomIndex * 2;
    value = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    return value & bits;
}

static inline u16 Ghost_NextLoopRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}

static inline u32 Ghost_OffsetLoopHeading(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}

// confidence=low: follows nine stored route points and retargets around the player; action role tentative.

void Ghost_FollowPlayerLoop(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *route = actor->context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        void *callback = actor->entryTarget228;
        u32 one = 1;
        actor->field2B2 = one;
        actor->field2B0 = 0;
        actor->field2B5 = 1;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = Ghost_FirstLoopRandom(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Ghost_FindPositionDelta(&delta, &actor->body.position, &target);
        Ghost_MakeLoopMotion(
            &route->motion,
            Ghost_OffsetLoopHeading(
                ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                &actor->tracking.heading) &
                255,
            32);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed = 1;
    }
    Ghost_FindPositionDelta(&delta, &actor->body.position, &route->points[route->index]);
    Ghost_EmitAfterimage(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->body.direction = Math_Atan2(route->points[route->index].x - actor->body.position.x,
                                           route->points[route->index].z - actor->body.position.z);
        Ghost_MakeLoopMotion(&delta, actor->body.direction, route->routeSpeed);
        actor->body.motion.x += delta.x;
        actor->body.motion.z += delta.z;
        route->motion = actor->body.motion;
    } else if (++route->index > 8) {
        Ghost_FindPositionDelta(&route->motion, &actor->body.position, &route->points[8]);
        route->motion.y = 0;
        actor->body.position.x = route->points[8].x;
        actor->body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        Ghost_MakeLoopMotion(
            &delta, Ghost_OffsetLoopHeading(Ghost_NextLoopRandom(), &actor->tracking.heading) & 255,
            512);
        target.x += delta.x;
        target.z += delta.z;
        Ghost_FindPositionDelta(&delta, &actor->body.position, &target);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed >>= 1;
        return;
    }
    if (actor->paletteOffset == 0) {
        if (Mod(actor->counter2AC, 16) == 0 && route->routeSpeed <= 11)
            ++route->routeSpeed;
    } else {
        if (Mod(actor->counter2AC, 8) == 0 && route->routeSpeed <= 11)
            route->routeSpeed += 2;
    }
    ++actor->counter2AC;
}

// confidence=low: follows nine stored route points and retargets around the player; action role tentative.

void Ghost_PursuePlayerLoop(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *route = actor->context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        void *callback = actor->entryTarget228;
        u32 one = 1;
        actor->field2B2 = one;
        actor->field2B0 = 0;
        actor->field2B5 = 1;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = Ghost_FirstLoopRandom(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Ghost_FindPositionDelta(&delta, &actor->body.position, &target);
        Ghost_MakeLoopMotion(
            &route->motion,
            Ghost_OffsetLoopHeading(
                ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                &actor->tracking.heading) &
                255,
            32);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed = 1;
    }
    Ghost_FindPositionDelta(&delta, &actor->body.position, &route->points[route->index]);
    Ghost_EmitAfterimage(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->body.direction = Math_Atan2(route->points[route->index].x - actor->body.position.x,
                                           route->points[route->index].z - actor->body.position.z);
        Ghost_MakeLoopMotion(&delta, actor->body.direction, route->routeSpeed);
        actor->body.motion.x += delta.x;
        actor->body.motion.z += delta.z;
        route->motion = actor->body.motion;
    } else if (++route->index > 8) {
        Ghost_FindPositionDelta(&route->motion, &actor->body.position, &route->points[8]);
        route->motion.y = 0;
        actor->body.position.x = route->points[8].x;
        actor->body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        Ghost_MakeLoopMotion(
            &delta, Ghost_OffsetLoopHeading(Ghost_NextLoopRandom(), &actor->tracking.heading) & 255,
            256);
        target.x += delta.x;
        target.z += delta.z;
        Ghost_FindPositionDelta(&delta, &actor->body.position, &target);
        Ghost_BuildMovementCurve(actor, &target);
        return;
    }
    if (Mod(actor->counter2AC, 16) == 0 && route->routeSpeed <= 17)
        ++route->routeSpeed;
    ++actor->counter2AC;
}

extern const u32 gUnk_08605DE8[];

void Ghost_InitActionTable(struct SceneActorRegistryResetView *actor)
{
    actor->behaviorCallbacks = gUnk_08605DE8;
}

static inline u16 SetFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *flags = &actor->flags2D4;
    return *flags = mask | *flags;
}

void Ghost_ApproachPlayer(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = Ghost_UpdateWaitingAnimation;
        u32 kind = 10;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        SetFlag(actor, 0x100);
        state->delay = state->delayDuration;
    }
    if (actor->field2B0)
        Ghost_StartMovementToPosition(actor, &gPlayers[0]->position);
    else
        actor->counter2AC++;
}

static inline void Ghost_SetDestinationState(struct SceneActorRegistryResetView *actor, u8 state,
                                             u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Ghost_ChooseMovementDestination(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *state = actor->context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    SceneActorControl_TakeFlag(&actor->field2B1);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if ((s32)actor->counter2AC > 60) {
        struct SceneActorRouteCursor *route;

        route = (void *)&state->route;
        Ghost_SelectRandomRouteEntry(route);
        target = &destination;
        target->x = route->point->x;
        target->z = route->point->z;
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            endpoint = target;
            target->y = SceneActorChase_HeightAt(Ghost_TerrainCell(cellX, cellZ), target);
        }

        Ghost_SetMovementEndpoints(actor, endpoint);
        Ghost_AdvanceMovementCurve(actor);
        Ghost_SetDestinationState(actor, 0, 0, 1);
        {
            struct ContactPosition *position = &actor->body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        actor->unknown3CD[0] = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(&state->motion);
        return;
    }
    actor->counter2AC++;
}

extern u8 gUnk_08605DF4[];

void Ghost_InitReactionTable(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = gUnk_08605DF4;
}

extern u32 gUnk_02000560;

static inline u32 Ghost_TestTriggerFlags(u32 mask, u32 *p)
{
    return *p & mask;
}

void Ghost_UpdateFacingTrigger(struct SceneActorRegistryResetView *actor)
{
    struct Ghost *context = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
    {
        u32 mask = 0x800;
        actor->flags2C0 = mask | actor->flags2C0;
    }
    actor->body.direction = actor->tracking.heading;
    if (!(gPadState[1] & A_BUTTON))
        goto wait;
    if (((u8 **)gPlayers)[0]) {
        u32 zero;
        s32 own = ((actor->body.direction + 0x20) & 0xFF) >> 5;
        s32 player = (((u8)gPlayers[0]->unk2D + 0x20) & 0xFF) >> 5;
        s32 diff = ((own + 4) & 7) - player;
        if (diff < 0)
            diff = -diff;
        if (diff <= 1 && actor->tracking.distance < context->triggerRange &&
            !(gEngineFlags & 0x200) && !(zero = Ghost_TestTriggerFlags(4, &gUnk_02000560))) {
            u32 two = 2;
            actor->state2A8 = two;
            actor->phase2A9 = zero;
            actor->behavior2AA = zero;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            Script_Call(context->effectScript, 0);
            return;
        }
    }
wait:
    actor->counter2AC++;
}

void Ghost_UpdateTriggerControl(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
    } else {
        {
            u32 mask = 0x800;
            actor->flags2C0 |= mask;
        }
        if (actor->counter2AC == 0)
            Trigger_ConsumePending(&actor->unknown9[3], 1);
        actor->counter2AC++;
    }
}

extern u8 gUnk_08605DFC[];

void Ghost_InitEffectTable(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks29C = gUnk_08605DFC;
}

void SceneActorControl_DispatchControl(u8 *);

u32 Ghost_OnBehaviorUpdate(struct SceneActorRegistryResetView *actor)
{
    u32 i;
    void **t;
    Ghost_UpdateVisibility(actor);
    i = actor->state2A8;
    t = actor->callbacks27C;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    ActorContact_UpdateColliderPositions(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Ghost_OnActorUpdate(struct SceneActorRegistryResetView *actor)
{
    actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActor_UpdateActiveCallbacks(actor);
    return 0;
}

/* Ghost_Destroy: Ivy_Destroy shape; removes the ghost auxiliary sprites first (ported by relatives.py, fixed by hand; provisional name) */

void Anim_Remove(void *);

void SceneMessages_UnregisterReceiver(void *);

void Shadow_Destroy(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void SceneActorRegistry_UnlinkActor(void *);

static inline u8 Ghost_TestDestroyFlags(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

u32 Ghost_Destroy(struct SceneActorRegistryResetView *actor)
{
    u8 *p;
    u8 *q;
    u32 m;
    u32 v;
    Ghost_RemoveAuxiliarySprites(actor);
    p = actor->drawable;
    if (actor->drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(&((struct SceneActorAnimationTileDrawable *)p)->tile);
    Heap_Free(p);
    m = 0x100;
    if (Ghost_TestDestroyFlags(&actor->flags2C0, m))
        SceneMessages_UnregisterReceiver(&actor->unknown9[3]);
    {
        u8 *r = actor->shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = ((struct SceneActorRegistryResetView *)actor)->flags2D0 & m;
    q = (void *)&actor->body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&actor->colliderA);
    SceneActorRegistry_UnlinkActor(actor);
    return 0;
}

void SceneActorRegistry_SetObjectContext(void *, void *, u32, void *);

s32 Route_GetCurrentPointFacing(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

void SceneActorPlacement_HideActor(void *, u32);

void SceneActorBehavior_InstallCallbackTables(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_SinkAndRemove(void);

void SceneActorReaction_ResolveDamage(void);

void SceneActorPlacement_PlayResponseAndResetState(void);

void SceneActorScript_OnCommand(void);

extern const u8 gUnk_08605E14[];

static inline void Ghost_SetInstanceFlags(struct Ghost *a, u32 mask)
{
    a->flags |= mask;
}

static inline u8 Ghost_TestInstanceFlags(struct Ghost *a, u32 mask)
{
    if (a->flags & mask)
        return 1;
    return 0;
}

static inline u8 Ghost_GetAnimationIndex(struct SceneActorRegistryResetView *actor)
{
    return actor->animationIndex;
}

static inline void Ghost_SetAnimationIndex(struct SceneActorRegistryResetView *actor, u32 value)
{
    actor->animationIndex = value;
}

static inline u8 Ghost_GetAnimationRestart(struct SceneActorRegistryResetView *actor)
{
    return actor->field2B3;
}

static inline void Ghost_InitializeScriptFields(struct SceneActorRegistryResetView *actor,
                                                u32 *fill)
{
    struct SceneActorBody *body = &actor->body;
    u32 id;
    struct ContactPosition origin;
    s32 first, second;
    s32 variant;
    actor->flags2CC = 0;
    actor->flags2D0 = 0;
    actor->enabled = 0;
    actor->field2E8 = 8;
    actor->flags2C0 = MonsterInit_ReadKeywordHalfword('M', 0);
    MonsterInit_ClearPosition(fill, &origin);
    id = Script_GetKeywordValue('N', 0);
    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(actor)->tracking);
    if (Script_SeekToKeyword('l')) {
        first = MonsterInit_ReadWord(300);
        second = MonsterInit_ReadWord(300);
    } else {
        first = 0;
        second = 0;
    }
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(actor, 0x200);
    actor->field1F0 = MonsterInit_ReadKeywordHalfword('F', 0);
    actor->field1EC = MonsterInit_ReadKeywordHalfword('R', 0);
    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(actor, MonsterInit_ReadHalfword(0));
        actor->field9E = MonsterInit_ReadByte(0);
        actor->field9F = MonsterInit_ReadByte(0);
        actor->fieldA0 = MonsterInit_ReadByte(0);
        actor->fieldA2 = MonsterInit_ReadHalfword(0);
        actor->fieldA4 = MonsterInit_ReadHalfword(0);
    }
    MonsterInit_InitializeHealth(actor, MonsterInit_GetInitialHealth(actor), first, second);
    if (Script_SeekToKeyword('X')) {
        actor->fieldAC = MonsterInit_ReadWord(0);
        actor->fieldA1 = MonsterInit_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&actor->fieldB2, MonsterInit_ReadKeywordHalfword('K', 2));
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    MonsterInit_WriteHalfword(&actor->field9A, variant);
    if (Script_GetKeywordValue('H', 0))
        MonsterInit_SetStatusFlags(actor, 0x800);
    SceneBody_Init(body, id, &origin, 0, 4, actor);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(actor) ? 0 : actor->drawable);
    SceneBody_SetMovementProbe(body, ((struct Ghost *)actor)->movementProbe, 0x40, 0x40);
}

static inline void Ghost_InstallPrimaryCallbacks(struct SceneActorRegistryResetView *actor,
                                                 void *cb274, u32 (*cb260)(void *), void *cb224,
                                                 void *cb228, void *cb22C, void *control,
                                                 void *cb230, void *cb234, void *defaultReaction,
                                                 void *targetQuery, void *damage, void *contact)
{
    u32 zero = 0;
    u32 mask = 0x2000;
    MonsterInit_SetBehaviorFlags(actor, mask);
    actor->contextCallback = cb274;
    MonsterInit_WriteAnimationQuery(&actor->animationQuery.word, cb260);
    actor->entryTarget224 = cb224;
    actor->entryTarget228 = cb228;
    actor->savedTarget = cb22C;
    actor->reactionTarget230 = cb230;
    actor->deathControl = cb234;
    actor->field220 = control;
    actor->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, 0);
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, 0);
    actor->callback248 = 0;
    actor->update26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->damageCallback = damage;
    actor->contactCallback = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Ghost_InitializeCallbacks(struct SceneActorRegistryResetView *actor)
{
    SceneActor_InitLinkedReactionColliders(actor);
    Ghost_InstallPrimaryCallbacks(
        actor, Ghost_OnBehaviorUpdate, Ghost_GetBehaviorCode, Ghost_TurnTowardHeading,
        Ghost_UpdateDirectionalPose, Ghost_UpdateDirectionalPoseB, 0, Ghost_UpdateHitStagger,
        Ghost_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    actor->callback248 = Ghost_ResetMovementState;
    actor->field23C = Ghost_OnControlCleanup;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Ghost_GetControlStatus);
    MonsterInit_WriteDataPointer(&actor->commandTable.opaque, gUnk_08605E14);
    MonsterInit_SetMotionFlags(actor, 0x20);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100400);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    {
        u32 disabled = 0;
        if (MonsterInit_TestMotionFlags(actor, 4)) {
            MonsterInit_SetStatusFlags(actor, 0x2000);
            disabled = 1;
        }
        if (disabled != 1) {
            MonsterInit_SetState(actor, 0, 0, 0);
            MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
        }
    }
    if (MonsterInit_TestMotionFlags(actor, 2))
        actor->regionId = gGameState->region;
    else
        actor->regionId = Region_FindContaining(&actor->body.position);
    if (MonsterInit_TestMotionFlags(actor, 0x40)) {
        struct ContactCollider *collider = &actor->colliderA;
        u32 mask = ~0x100;
        collider->flags = mask & collider->flags;
    }
}

static inline void Ghost_ReadPalette(struct SceneActorRegistryResetView *actor, u32 base,
                                     u32 alternate)
{
    s32 selected;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword('C', 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    actor->alternateOffset = MonsterInit_GetPaletteOffset(actor) + base;
    selected = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (!MonsterInit_GetDrawableKind(actor))
        AnimData_SelectPaletteBank(&((struct GhostVisual *)actor->drawable)->sequence.animation,
                                   selected);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
            gPaletteBankData + (selected << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
            SceneActorScript_GetManagerField62());
    actor->alternatePalette = MonsterInit_GetPaletteBase(actor);
    actor->paletteBank = MonsterInit_GetPaletteBase(actor) + 2;
    actor->field1EA = 0x1EE;
}

static inline void Ghost_InstallCompletion(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Ghost_OnActorUpdate;
    actor->destroyCallback = Ghost_Destroy;
}

static inline void Ghost_SelectAnimation(struct SceneActorRegistryResetView *actor, u32 selection,
                                         u32 variant, u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = (void *)actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        selection += variant;
        if (!force && Ghost_GetAnimationIndex(actor) == selection &&
            !Ghost_GetAnimationRestart(actor)) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        Ghost_SetAnimationIndex(actor, selection);
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    actor->field2B3 = 0;
}

static inline void Ghost_InitializeDrawable(struct SceneActorRegistryResetView *actor, u32 *fill)
{
    struct ContactPosition zero;
    if (!MonsterInit_GetDrawableKind(actor)) {
        u8 *d;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        d = MonsterInit_Drawable(actor);
        AnimData_Load(&((struct GhostVisual *)d)->sequence.animation, 0xD637);
        Sprite_Init(d, &((struct GhostVisual *)d)->sequence.animation, 0);
        SceneActorAnimation_InitEnemySequence(d, 0x29FB);
        if (actor->category == 0x11 || actor->category == 0x17)
            ((struct ActorSprite *)d)->priority = 1;
        actor->body.direction = 0;
    } else {
        u8 *d;
        void *res;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        d = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(fill, &zero);
        res = ScriptProc_Start(0xCB05, 0xD637);
        *(struct DisplayResourceHeader *)d = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(&((struct SceneActorAnimationTileDrawable *)d)->tile, d, 0, 0, 2, 2, 0x3C,
                          &zero);
        actor->animationIndex = 0xFF;
    }
}

static inline u32 Ghost_ReadRoute(struct SceneActorRegistryResetView *actor,
                                  struct SceneActorRegistryRoute *route)
{
    struct ContactPosition point;
    route->activeBranch = 0;
    if (Script_SeekToKeyword('r')) {
        route->routeId = MonsterInit_ReadWord(0);
        route->parameterB = 0;
        route->pointId = MonsterInit_ReadWord(0);
        route->currentPoint = route->pointId;
        route->branches[MonsterInit_RouteActiveBranch(route)] = route->routeId;
        route->branches[MonsterInit_RouteAlternateBranch(route)] =
            Script_GetPc() ? Script_GetValue() : MonsterInit_RouteId(route);
        Path_GetPointPositionById((u16 *)&point, route->routeId, route->pointId);
        actor->terrainCell = MonsterInit_CellIndex(&point);
        point.y = actor->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &point);
        MonsterInit_BeginRoute(&actor->body, route);
        return 1;
    } else {
        route->routeId = 0;
        route->parameterB = 0;
        route->pointId = 0;
        route->currentPoint = 0;
        route->branches[MonsterInit_RouteActiveBranch(route)] = 0;
        route->branches[MonsterInit_RouteAlternateBranch(route)] = 0;
    }
    return 0;
}

s32 Ghost_Init(void *object)
{
    struct SceneActorRegistryResetView *a = object;
    struct Ghost *arg0 = object;
    u32 fill;
    u32 row, frameFlags;
    struct SceneActorRegistryRoute *route;
    struct SceneActorProfileEntry *profile;
    arg0->flags = 0;
    SceneActorRegistry_SetObjectContext(a, a, 0, 0);
    {
        u8 category = 0x17;
        a->category = category;
    }
    Ghost_InitActorProfile(a);
    Ghost_InitializeDrawable(a, &fill);
    Ghost_InitializeScriptFields(a, &fill);
    route = &arg0->route;
    if (Ghost_ReadRoute(a, route)) {
        {
            struct ContactPosition *dest = &a->body.position;
            struct SceneActorRegistryRoute *link = route;
            dest->x = MonsterInit_RoutePointXRaw(link);
            dest->z = MonsterInit_RoutePointZRaw(link);
            dest->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(dest), dest);
        }
        MonsterInit_DrawablePosition(a);
        a->body.direction = Route_GetCurrentPointFacing(route);
    }
    if (Script_SeekToKeyword('p')) {
        a->body.position.x = MonsterInit_ReadHalfword(0);
        a->body.position.y = MonsterInit_ReadHalfword(0);
        a->body.position.z = MonsterInit_ReadHalfword(0);
        a->terrainCell = MonsterInit_CellIndex(&a->body.position);
        a->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(a), &a->body.position);
        arg0->baseHeight = a->groundHeight2BE;
        MonsterInit_DrawablePosition(a);
    } else
        arg0->baseHeight = a->groundHeight2BE;
    arg0->delayDuration = MonsterInit_ReadKeywordHalfword('l', 0);
    arg0->previousPlayer = *(struct ContactPosition *)&gPlayers[0]->position;
    if (Script_SeekToKeyword('P')) {
        arg0->drainInterval = MonsterInit_ReadByte(0);
        arg0->drainDuration = MonsterInit_ReadByte(0);
        arg0->particleStyle = MonsterInit_ReadByte(0);
    }
    if (Script_GetKeywordValue('g', 0))
        Ghost_SetInstanceFlags(arg0, 0x100);
    if (Script_SeekToKeyword('d')) {
        arg0->effectScript = MonsterInit_ReadHalfword(0);
        MonsterInit_ReadScriptWord(&arg0->triggerRange, 0);
        arg0->triggerRange *= arg0->triggerRange;
    } else {
        arg0->effectScript = 0;
        arg0->triggerRange = 0;
    }
    if (Script_GetKeywordValue('i', 0))
        Ghost_SetInstanceFlags(arg0, 0x200);
    Ghost_ReadPalette(a, 0x1EB, 0x192);
    if (!Script_GetKeywordValue('U', 0)) {
        struct Ghost *context = a->context;
        s32 enabled;
        if (Ghost_TestInstanceFlags(context, 0x100))
            enabled = MonsterInit_TestWorldFlags(0x20);
        else
            enabled = gGameState->unk876;
        if (enabled > 0) {
            Ghost_SetInstanceFlags(context, 0x40);
            ((struct ActorSprite *)a->drawable)->flags |= 1;
            context->visibilityTimer = 0;
        } else {
            Ghost_SetInstanceFlags(context, 0x20);
            ((struct ActorSprite *)a->drawable)->flags &= ~1;
            context->visibilityTimer = 0;
        }
    }
    Ghost_InitializeCallbacks(a);
    if (Script_GetKeywordValue('S', 0) || !MonsterInit_IsInCurrentRegion(a))
        SceneActorPlacement_HideActor(a, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    if (!MonsterInit_GetDrawableKind(a))
        DisplaySequence_Step(a->drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile, a->drawable);
    MonsterInit_InitializeReceiver(a);
    SceneActorBehavior_InstallCallbackTables(a);
    Ghost_InitMotionPreset(a);
    Ghost_InitActionTable(a);
    Ghost_InitReactionTable(a);
    if (arg0->effectScript) {
        Ghost_InitEffectTable(a);
        MonsterInit_SetStatusFlags(a, 8);
    }
    profile = (void *)SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 1);
    Ghost_GetDirectionalFrame(a->body.direction, &row, &frameFlags);
    Ghost_SelectAnimation(a, profile->value, row, frameFlags);
    a->field2E8 = 2;
    SceneActor_InitLinkedSprites(a);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(a), a);
    a->saved.privateFlags = arg0->flags;
    Ghost_InstallCompletion(a);
    if (!SceneActorRegistry_LinkActor(a))
        return -1;
    return 0;
}

void Ghost_Create(void)
{
    void *m = Heap_Alloc(0x62c);
    if (m != 0) {
        Memory_Clear(m, 0x62c);
        if (Ghost_Init(m) < 0) {
            Ghost_Destroy(m);
            Heap_Free(m);
        }
    }
}
