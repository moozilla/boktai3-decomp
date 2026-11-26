#include "monsters/ghost.h"
#include "engine/math/math_shift.h"
#include "scene/collision/scene_actor_stimulus.h"
#include "system/game_state/game_state_flags.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_turn.h"
#include "engine/word_flags.h"

#include "global.h"

#include "scene/actor/scene_actor_reset.h"

#include "player/player.h"

#include "scene/actor/scene_actor_state.h"

#include "scene/motion/scene_actor_chase_reaction.h"

#include "scene/motion/scene_actor_terrain.h"

#include "npc/manager/scene_actor_animation.h"

#include "engine/engine.h"

#include "engine/ram.h"

#include "scene/actor/scene_actor_control_phase.h"

#include "scene/motion/scene_actor_chase_shared.h"

#include "script/script_utility.h"

#include "scene/motion/scene_actor_route.h"

#include "scene/actor/scene_actor_visibility.h"

#include "scene/actor/scene_actor_control_helpers.h"

#include "engine/engine_globals.h"

#include "scene/collision/contact_actor.h"

#include "engine/math/movement_curve.h"

#include "scene/actor/scene_actor_control_motion.h"

#include "engine/video/display_sequence_inline.h"

#include "npc/cutscene_actor/actor_animation_controller.h"

#include "debug.h"

#include "scene/actor/scene_actor_behavior.h"

#include "monsters/scene_route_sample.h"

#include "engine/math/monster_particle_random.h"

#include "engine/input.h"

#include "gba/io_reg.h"

#include "scene/collision/scene_body_api.h"

#include "engine/video/render.h"

#include "system/game_state/game_state.h"

#include "monsters/scene_actor_init_group.h"

#include "monsters/scene_actor_init_graphics.h"
#include "scene/collision/scene_actor_contact_response.h"
#include "bosses/vanargand/vanargand_projectiles.h"
#include "scene/motion/scene_actor_displacement.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/motion/scene_actor_chase.h"
#include "objects/hazards/falling_objects.h"
#include "bosses/vanargand/vanargand_parts.h"

/* The Ghost context flag word. */
static inline u8 Ghost_TestFlags(struct Ghost *a, u32 mask)
{
    if (a->flags & mask)
        return 1;
    return 0;
}

static inline void Ghost_SetFlags(struct Ghost *a, u32 mask)
{
    a->flags |= mask;
}

static inline void Ghost_ClearFlags(struct Ghost *context, u32 bits)
{
    u32 mask = ~bits;
    context->flags &= mask;
}

/* Empty callback; callers have not been identified. */

void Ghost_NoOp(void)
{
}


// confidence=low: contact type 1 interrupts control and starts a linked-player reaction.

void Ghost_BeginContactReaction(void *unused, struct ContactCollider *contact,
                                struct ChaseActor *input)
{
    struct ChaseActor *s = input;
    if (!SceneActor_ActionsBlocked(s)) {
        if (!SceneActorChase_QueryMotionFlagComponent(s, 0x4000) && contact->contactFlags == 1) {
            struct Ghost *p = s->base.context;
            u8 linked;
            SceneActorChase_SetControl(s, SceneActorControl_OnIdle, 0x10);
            SceneActorChase_SetState(s, 0, 2, 0);
            EffectCollision_Disable(&s->base.colliderB);
            EffectCollision_Disable(&s->base.colliderA);
            SceneActorChase_ClearBits(&s->base.reactionFlags, 0x180);
            /* The linked flag is read here, but the result goes unused. */
            linked = Ghost_TestFlags(p, 0x80);
            {
                struct Player *player = gPlayers[0];
                u32 three;
                three = 3;
                player->movementSpeedReduction += 3;
                Ghost_SetFlags(p, 0x80);
                p->revealTicks = three;
            }
            Sound_PlaySong(0x3CB);
        }
    }
}

/* SceneActorBehavior_OnContactForward: forwards the contact unless the actor is unavailable or flagged */

void SceneActorBehavior_ApplyContactReaction(void *, void *, struct ChaseActor *);

void SceneActorBehavior_OnContactForward(u16 *source, void *destination, struct ChaseActor *actor)
{
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (SceneActorChase_QueryMotionFlagComponent(actor, 0x4000))
        return;
    if (SceneActor_MaskedHalfword(0x800, &source[3]))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
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

static inline void Ghost_SelectDirectionalAnimation(struct RoomActor *actor, u32 selection,
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
        struct SceneInputWidget *tile =
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

static inline s32 Ghost_UnwrapProgressCounter(s32 n)
{
    return n - 256;
}

// confidence=low: drains player energy while tracking the player's wrapping progress counter.

void Ghost_UpdateEnergyDrainAnimation(struct RoomActor *actor)
{
    struct Ghost *body = actor->context;
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    u32 variant, flags, selection;
    s32 delta;
    u8 *elapsed;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        body->drainStart = gPlayers[0]->facingChangeEffects;
        body->drainCounter = 0;
        ((struct ChaseActor *)actor)->base.field2C2 = 16;
    }
    Ghost_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
    if (((struct ChaseActor *)actor)->base.paletteOffset == 0) {
        if (Mod(((struct ChaseActor *)actor)->base.frameCounter, body->drainInterval) == 0)
            Player_SubtractEnergy(((u8 **)gPlayers)[0],
                                  ((struct ChaseActor *)actor)->base.attacks.first[0]);
    } else if (((struct ChaseActor *)actor)->base.paletteOffset == 1) {
        if (Mod(((struct ChaseActor *)actor)->base.frameCounter, body->drainInterval) == 0)
            Player_SubtractEnergy(((u8 **)gPlayers)[0],
                                  ((struct ChaseActor *)actor)->base.attacks.first[0]);
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
            SceneActorChase_SetReactionFlags(actor, 32);
        *elapsed = delta;
    }
    if (body->drainElapsed > body->drainDuration)
        actor->stepDone = 1;
}

void Ghost_UpdateDirectionalPose(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (actor->field2B2)
        actor->field2B2 = 0;
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    Ghost_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
}

void Ghost_UpdateDirectionalPoseB(struct RoomActor *actor)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        ((struct ChaseActor *)actor)->base.field2C2 = 0x40;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    Ghost_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
}

// fix.py best version (reprinted C)

// Remaining differences include octant-mask lifetime, control initialization and registers.

// confidence=low: turns one octant at animation events until reaching the requested heading.

void Ghost_TurnTowardHeading(struct ChaseActor *actor)
{
    struct SceneActorBody *motion = &actor->base.body;
    struct ActorAnim *sequence;
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
    sequence = &((struct GhostVisual *)actor->base.drawable)->sequence.anim;
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
            Anim_SetDurationScale(&((struct GhostVisual *)actor->base.drawable)->sequence.anim,
                                  *scale);
        else
            Anim_SetFrameDurationScale(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
    }
    if (actor->base.frameCounter & 4) {
        u32 variant;
        u32 flags;
        u32 selection;
        SceneActorChase_SetControl(actor, actor->base.onEntry, 2);
        if (SceneActorTurn_ActorFlags(actor, 0x10000))
            motion->direction += 32;
        else
            motion->direction -= 32;
        Ghost_GetDirectionalFrame(motion->direction, &variant, &flags);
        sequence->count = 0;
        selection = *((u8 *)SceneActorProfiles_GetEntryAddress(actor->base.profileId,
                                                               actor->base.profileMode, 1));
        Ghost_SelectDirectionalAnimation((void *)actor, selection, variant, flags);
    }
}

// confidence=low: selects Ghost's waiting animation and marks the wait complete after 60 ticks.

void Ghost_UpdateWaitingAnimation(struct RoomActor *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry;
    u32 variant, flags, selection;
    if (actor->field2B2)
        actor->field2B2 = 0;
    Ghost_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 11);
    selection = entry->value;
    Ghost_SelectDirectionalAnimation(actor, selection, variant, flags);
    if (frame <= 29)
        SceneActorChase_SetReactionFlags(actor, 32);
    if (frame > 59)
        actor->stepDone = 1;
}

// confidence=low: recoil callback selects animation 15 and completes after sixty ticks.

void Ghost_UpdateRecoilAnimation(struct ChaseActor *actor, s32 tick)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
    }
    if (tick <= 29) {
        SceneActorChase_SetReactionFlags(actor, 32);
    }
    if (tick > 59) {
        actor->base.stepDone = 1;
    }
}

void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);

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

void Ghost_UpdateHitStagger(struct ChaseActor *actor, s32 tick)
{
    struct Ghost *ctx = actor->base.context;
    struct ContactPosition range;
    s32 x, z;
    s16 *priorX, *priorZ;
    u16 *f;
    u8 taken;
    taken = SceneActorControl_TakeFlag(&actor->base.field2B2);
    f = &actor->base.motionFlags;
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
            SparkParticle_Spawn(&actor->base.body.position, Ghost_RandomSparkCount(), &range,
                                actor->base.body.direction + 128, 10, 6, 2, 2);
        }
    }
    {
        s32 current = actor->base.reactionStart.x;
        s16 *field = &actor->base.reactionEnd.x;
        s32 difference = current - *field;
        priorX = field;
        x = SceneActorDisplacement_Quarter(difference);
    }
    {
        s32 current = actor->base.reactionStart.z;
        s16 *field = &actor->base.reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = SceneActorDisplacement_Quarter(difference);
    }
    x = Ghost_ClampMotion(x);
    z = Ghost_ClampMotion(z);
    actor->base.body.motion.x += x;
    actor->base.body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (Ghost_IsMotionAtRest(x, z)) {
        actor->base.stepDone = 1;
        Ghost_EndPlayerSlowdown(actor);
        {
            struct GhostSignedFlags16 *flags = (void *)f;
            s32 m = ~0x28;
            m &= (u16)flags->bits;
            flags->bits = m;
        }
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
        MonsterInit_SetStatusFlags(actor, 0x100000);
        SceneActor_EndLinkedReaction(actor);
        Ghost_SetFlags(ctx, 0x10);
        {
            u16 v = ctx->delayDuration;
            ctx->delay = v;
        }
        SceneActorTerrain_EnterState(actor, 0, 2, 2);
    }
}

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

void EffectManager_SpawnParticlesAt(u32, u32, u32, void *, void *, u32, u32);

static inline u32 Ghost_ReadDefeatFlags(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.motionFlags & mask;
}

static inline void Ghost_SetScriptValue(s32 value, s32 *entry)
{
    *entry = value;
}

// confidence=low: defeat control; emits debris sprites, runs the defeat script, then returns to idle.

void Ghost_UpdateDefeat(struct ChaseActor *actor)
{
    struct Ghost *ctx = actor->base.context;
    struct {
        u16 x;
        u16 y;
        u16 z;
        u16 w;
    } pieces[3];
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        actor->base.colliderA.flags |= 4;
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
            pos = &actor->base.body.position;
            {
                u16 *x = (u16 *)&pos->x;
                u16 *y = (u16 *)&actor->base.body.position.y;
                u16 *z = (u16 *)&actor->base.body.position.z;
                for (i = 0; i < 3; i++) {
                    pieces[i].x = *x;
                    pieces[i].y = *y;
                    pieces[i].z = *z;
                }
            }
            EffectManager_SpawnParticlesAt(count, 0, ctx->particleStyle, pos, pieces, 300, 120);
        }
    }
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = actor->base.script1F8;
    if (script) {
        s32 values[8];
        struct ScriptUtilityArguments arguments;
        Ghost_SetScriptValue(actor->base.body.position.x, &values[0]);
        Ghost_SetScriptValue(actor->base.body.position.y, &values[1]);
        Ghost_SetScriptValue(actor->base.body.position.z, &values[2]);
        Ghost_SetScriptValue(actor->base.parameters1FC[0], &values[3]);
        Ghost_SetScriptValue(actor->base.parameters1FC[1], &values[4]);
        Ghost_SetScriptValue(actor->base.parameters1FC[2], &values[5]);
        Ghost_SetScriptValue(actor->base.hitOwners, &values[6]);
        Ghost_SetScriptValue(actor->base.body.id, &values[7]);
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        u16 *flags;
        u16 masked;
        u32 test = actor->base.motionFlags & mask;
        flags = &actor->base.motionFlags;
        if ((!test) && (((u32)(actor->base.category - 11)) > 2))
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        masked = SceneActor_MaskedHalfword(0x4000, flags);
        if (!masked) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            SceneActorChase_SetState(actor, 1, 0, 2);
        }
    }
}

static inline void Ghost_MakeRouteMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 255] * speed, 12);
}

void Centipede_ResetOscillation(void *);

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

void Centipede_SelectWanderRoute(void *);

void Centipede_AdvanceMotionPhase(void *, s32);

void sub_080A9DB0(void *, s32, s32);

void Centipede_UpdateOscillation(void *, s32, s32);

static inline u8 Ghost_ApproachCurvePosition(struct ChaseActor *actor,
                                             struct SceneActorMotionSubject *saved, s32 speed)
{
    struct SceneActorBody *motion = &actor->base.body;
    s16 dx = saved->position.x - (u16)motion->position.x;
    s16 dz = saved->position.z - (u16)motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        *(u16 *)&motion->motion.x +=
            Math_ShiftTowardZero(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        *(u16 *)&motion->motion.z +=
            Math_ShiftTowardZero(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

static inline void Ghost_UpdateAnimationSpeed(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(&((struct GhostVisual *)actor->base.drawable)->sequence.anim, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
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

void Ghost_FollowRoute(struct ChaseActor *actor)
{
    struct Ghost *body = actor->base.context;
    s32 speed;
    struct ContactPosition target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&body->motion);
        body->curveDirection = 1;
        actor->base.field2C2 = 64;
    }
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
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
        Ghost_MakeRouteMotion((void *)(&body->motion), actor->base.body.direction, speed);
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
    actor->base.counter2AC++;
}

// confidence=low: decays saved horizontal route motion before returning to pursuit or rest.

static inline void Ghost_SetDecelerationState(struct ChaseActor *actor, u32 phase, u32 behavior,
                                              u32 zero)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = zero;
    actor->base.field2B1 = 1;
}

void Ghost_DecelerateMovement(struct ChaseActor *actor)
{
    struct Ghost *ctx = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SceneActorChase_SetControl(actor, actor->base.onEntry, 2);
    actor->base.body.motion.x += ctx->motion.x;
    actor->base.body.motion.z += ctx->motion.z;
    if (Mod(actor->base.counter2AC, 4) == 0) {
        ctx->motion.x >>= 1;
        ctx->motion.z >>= 1;
    }
    if (actor->base.stepDone) {
        u16 flags = SceneActor_MaskedHalfword(0x100, &actor->base.motionFlags);
        if (flags)
            Ghost_SetDecelerationState(actor, 2, 1, 0);
        else
            Ghost_SetDecelerationState(actor, 0, 0, flags);
    } else
        actor->base.counter2AC++;
}

void Ghost_ChooseRouteDestination(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        state->flightPhase = 0;
    }
    if ((s32)actor->base.counter2AC > 60) {
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
        SceneActorTerrain_EnterState(actor, 0, 0, 2);
        {
            struct ContactPosition *position = &actor->base.body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        actor->base.nextDirection = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(&state->motion);
    }
    actor->base.counter2AC++;
}

static inline void Ghost_GetRouteEndpoint(struct GhostRouteCursor *path,
                                          struct ContactPosition *pos)
{
    struct SceneActorRoutePoint *point = path->point;
    pos->x = point->x;
    pos->z = path->point->z;
    pos->y = SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(pos), pos);
}

// confidence=low: starts a random recoil then returns to the stored terrain endpoint.

void Ghost_RecoilTowardEndpoint(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
    struct ContactPosition pos;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 angle;
        u32 n;
        SceneActorChase_SetControl(actor, Ghost_UpdateRecoilAnimation, 0x24);
        Ghost_EndPlayerSlowdown(actor);
        gRandomIndex = n = (gRandomIndex + 1) & 1023;
        angle = *(u8 *)&((const u16 *)RANDOM_TABLE_ADDR)[n];
        ZeroFill8(&context->motion);
        actor->base.secondaryMotion.x = SceneRoute_ScaleComponent(gUnk_086149C4[angle] * 64);
        actor->base.secondaryMotion.z =
            SceneRoute_ScaleComponent(gUnk_086149C4[(angle + 64) & 255] * 64);
        actor->base.body.direction =
            Math_Atan2(actor->base.secondaryMotion.x, actor->base.secondaryMotion.z) - 128;
    }
    if (actor->base.stepDone) {
        Ghost_ClearActionFlags(actor);
        ZeroFill8(&actor->base.secondaryMotion);
        MonsterInit_SetState(actor, 0, 0, 2);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        {
            struct GhostRouteCursor *path = &context->route;
            Ghost_SelectRandomRouteEntry(path);
            Ghost_GetRouteEndpoint(path, &pos);
        }
        Ghost_StartMovementToPosition(actor, &pos);
    } else
        ++actor->base.counter2AC;
}

static inline void Ghost_MakePolarMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = SceneRoute_ScaleComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = SceneRoute_ScaleComponent(gUnk_086149C4[angle & 255] * speed);
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

static inline void Ghost_SetState(struct ChaseActor *actor, u8 phase, u8 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline s32 Ghost_QuarterDistance(s32 n)
{
    if (n >= 0)
        return n >> 2;
    return -(-n >> 2);
}

static inline u8 Ghost_TestRouteFlags(struct Ghost *route, u32 mask)
{
    u32 flags = route->flags;
    if (flags & mask)
        return 1;
    return 0;
}

static inline void Ghost_ClearMotionFlags(struct ChaseActor *actor, u32 bits)
{
    u16 *flags = &actor->base.motionFlags;
    u32 mask = ~bits;
    *flags &= mask;
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
    x = SceneRoute_ScaleComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    zero = 0;
    GHOST_WRITE_COMPONENT(out, x, x);
    GHOST_SET_VERTICAL(out, zero);
    z = SceneRoute_ScaleComponent(gUnk_086149C4[angle & 255] * speed);
    GHOST_WRITE_COMPONENT(out, z, z);
}

// confidence=low: follows a finite curved route toward or away from the player.

void Ghost_AdvanceApproachRoute(struct ChaseActor *actor)
{
    struct Ghost *route = actor->base.context;
    struct ContactPosition target;
    struct ContactPosition delta;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        route->index = 0;
        if (Ghost_TestRouteFlags(route, 4)) {
            Ghost_MakeRetreatOffset(
                &target, (actor->base.tracking.heading + 128 + (Ghost_NextRandom() & 63)) & 255);
            Ghost_AddPosition(&target, &actor->base.body.position);
        } else {
            target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        }
        Chase_Delta(&delta, &actor->base.body.position, &target);
        route->approachDistance = Ghost_QuarterDistance(Ghost_LengthSquared(&delta));
        Ghost_MakePolarMotion(
            &route->motion,
            Ghost_OffsetHeading(Ghost_NextMotionRandom(), &actor->base.tracking.heading) & 255, 32);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed = 1;
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        actor->base.field2C2 = 32;
    }
    Chase_Delta(&delta, &actor->base.body.position, &route->points[route->index]);
    if (!Ghost_TestRouteFlags(route, 4))
        MonsterInit_MaskColliderFlags(&actor->base.colliderB, ~4);
    Ghost_EmitAfterimage(actor);
    if ((Ghost_LengthSquared(&delta)) > 1024) {
        actor->base.body.direction =
            Math_Atan2(route->points[route->index].x - actor->base.body.position.x,
                       route->points[route->index].z - actor->base.body.position.z);
        Ghost_MakePolarMotion(&delta, actor->base.body.direction, route->routeSpeed);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        route->motion = actor->base.body.motion;
    } else if ((++route->index) > 8) {
        actor->base.body.position.x = route->points[8].x;
        actor->base.body.position.z = route->points[8].z;
        Ghost_ClearMotionFlags(actor, 0x100);
        ContactCollider_SetFlags(&actor->base.colliderB, 4);
        if (Ghost_TestRouteFlags(route, 4)) {
            Ghost_SetState(actor, 0, 0);
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
            Ghost_ClearFlags(route, 4);
        } else {
            Ghost_SetState(actor, 2, 1);
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        }
        return;
    }
    if (actor->base.paletteOffset == 0) {
        if ((Mod(actor->base.counter2AC, 8) == 0) && (route->routeSpeed <= 19))
            ++route->routeSpeed;
    } else if ((Mod(actor->base.counter2AC, 4) == 0) && (route->routeSpeed <= 19))
        route->routeSpeed += 2;
}

extern u8 gUnk_08605DD4[];

void Ghost_InitMotionPreset(struct ChaseActor *actor)
{
    actor->base.callbacks284 = gUnk_08605DD4;
}

void SceneActor_BeginLinkedIdleReaction(struct ChaseActor *actor)
{
    if (!SceneActor_ActionsBlocked(actor) && Ghost_TestFlags(actor->base.context, 0x80)) {
        SceneActorTerrain_EnterState(actor, 0, 0, 3);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        actor->base.colliderA.flags &= 0xFFFB;
    }
}

/* actor must be nonnull: its control context is loaded before the availability gate. */

void Ghost_RequestVisibility(struct ChaseActor *input, const u16 *inputRequest)
{
    struct ChaseActor *actor = input;
    const u16 *request = inputRequest;
    struct Ghost *state = actor->base.context;
    u32 unavailable = SceneActor_ActionsBlocked(actor);
    if (unavailable)
        return;
    if (!SceneActor_MaskedHalfword(0x4000, &actor->base.motionFlags))
        return;
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
                            *savedFlags = old | 0x20;
                        }
                    }
                    Sound_PlaySong(0x21E);
                    {
                        u32 mask = 0x800;
                        u16 *flags;
                        flags = &actor->base.placementFlags;
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
                            *savedFlags = old | 0x40;
                        }
                    }
                    Sound_PlaySong(0x21E);
                    {
                        u32 mask = 0x800;
                        u16 *flags;
                        flags = &actor->base.placementFlags;
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

void SceneActor_EasePlayerHeight(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
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
    if (actor->base.controlKind == 0x23) {
        target = gPlayers[0]->position.y + offset;
        Ghost_EaseHeight(target, &actor->base.body.position.y);
    } else {
        s32 wave = gUnk_086149C4[state->flightPhase] << 5;
        target = gPlayers[0]->position.y + offset + VanargandProjectile_Scale(wave);
        Ghost_EaseHeight(target, &actor->base.body.position.y);
        state->flightPhase += 2;
    }
}

void Ghost_UpdateCurveSway(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
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

u8 Ghost_SelectRandomRouteEntry(struct SceneActorRouteCursor *cursor)
{
    struct SceneActorRouteCursor *result = cursor;
    u8 pointIndex;
    if (cursor->definition->pointCount <= 1)
        return 0;
    pointIndex = Mod(FallingObject_Random(), cursor->definition->pointCount);
    if (pointIndex == cursor->pointIndex)
        pointIndex = Mod(pointIndex + 1, cursor->definition->pointCount);
    result->pointIndex = pointIndex;
    result->unknown3 = 0;
    result->point = (void *)(Path_GetPointData((void *)result->definition) +
                             result->pointIndex * sizeof(*result->point));
    return 1;
}

struct ChaseActor;

void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void SceneActor_InitLinkedReactionColliders(struct ChaseActor *actor)
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
        struct ContactCollider *collider = &actor->base.colliderB;
        Collider_Init(collider, actor->base.body.id, flags, 0, 16, &size, &offset);
        Collider_SetAttackProperties(&actor->base.colliderB, 0, 0, 0, 0, 0);
        Collider_SetPosition(&actor->base.colliderB, &actor->base.body.position, 0);
        Collider_SetCallback(&actor->base.colliderB, Ghost_BeginContactReaction, actor);
    }
}

s32 Ghost_GetBehaviorCode(void)
{
    return 4;
}

void Ghost_SetMovementEndpoints(struct ChaseActor *actor, struct ContactPosition *destination)
{
    struct Ghost *context = actor->base.context;
    context->start = actor->base.body.position;
    context->end = *destination;
    context->curveTime = 0;
}

u8 Ghost_AdvanceMovementCurve(struct ChaseActor *actor)
{
    struct Ghost *ctx = actor->base.context;
    struct ContactPosition start, end;
    start = ctx->start;
    start.y = 0;
    end = ctx->end;
    end.y = 0;
    MovementCurve_InterpolatePosition(&ctx->curveSubject.position, &start, &end, 1, ctx->curveTime,
                                      0, ctx->curve);
    if (ctx->curveTime > 1023)
        return 1;
    ctx->curveTime += 128;
    return 0;
}

void Ghost_InitActorProfile(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(0x17, 0);
}

void Ghost_ResetMovementState(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
    context->curveTime = 0;
    context->flightPhase = 0;
    context->curveDirection = 1;
    context->curve = 0;
    context->drainElapsed = 0;
    context->drainStart = 0;
    context->flags = actor->base.saved.privateFlags;
    *(struct GhostPackedPosition *)&context->previousPlayer =
        *(struct GhostPackedPosition *)&gPlayers[0]->position;
    ZeroFill8(&context->start);
    ZeroFill8(&context->end);
    ZeroFill8(&context->motion);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void SceneActor_EndLinkedReaction(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
    MonsterInit_SetStatusFlags(actor, 0x100000);
    SceneActorChase_ClearWordFlags(&state->flags, 0x11);
    {
        struct ContactCollider *collider = &actor->base.colliderA;
        collider->flags &= 0xFFFB;
    }
    {
        u32 mask = 0x80;
        if (SceneActorChase_Test(state->flags, mask)) {
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

void Ghost_StartMovementToPosition(struct ChaseActor *actor, s16 *destination)
{
    struct Ghost *context = actor->base.context;
    struct ContactPosition *target = (void *)destination;
    struct ContactPosition delta;
    Ghost_SetMovementEndpoints(actor, target);
    Ghost_AdvanceMovementCurve(actor);
    {
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        Chase_Delta(&delta, &actor->base.body.position, target);
        actor->base.nextDirection = Math_Atan2(delta.x, delta.z);
        ZeroFill8(&context->motion);
    }
}

void Ghost_ClearActionFlags(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
    u32 *actionFlags;
    u32 actionMask;
    struct SceneActorChaseTracking *tracking;
    u32 trackingMask;
    SceneActorChase_ClearMotionFlagComponent(actor, 0x100);
    tracking = &actor->base.tracking;
    trackingMask = ~8;
    tracking->flags = tracking->flags & trackingMask;
    actionFlags = &((struct Ghost *)actor->base.context)->flags;
    actionMask = ~0x10;
    *actionFlags = *actionFlags & actionMask;
    context->delay = 0;
}

// confidence=low: when the idle flag is set and the delay expires, picks a random route point and starts moving to it.

static inline u16 Ghost_TestDestinationFlags(struct ChaseActor *actor, u32 mask)
{
    mask &= actor->base.motionFlags;
    return mask;
}

void Ghost_UpdateRandomDestination(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (Ghost_TestDestinationFlags(actor, 0x120))
        return;
    {
        u8 ready = Ghost_TestFlags(state, 0x10);
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
            actor->base.state2A8 = zero;
            actor->base.phase2A9 = zero;
            actor->base.behavior2AA = two;
            actor->base.counter2AC = zero;
            actor->base.field2B1 = 1;
        }
    }
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

static inline void Ghost_SetPlayerReactionState(struct ChaseActor *actor, u8 state, u8 phase,
                                                u32 behavior, u32 counter)
{
    u32 selected = behavior;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = selected;
    actor->base.counter2AC = counter;
    actor->base.field2B1 = 1;
}

// confidence=low: reacts to player motion within range, with separate drain and approach transitions.

void Ghost_ReactToPlayerMovement(struct ChaseActor *actor)
{
    struct Ghost *state;
    struct ContactPosition delta;
    s32 height;
    u32 active;
    u32 *savedFlags;
    struct SceneActorReactionMotion *reaction;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (SceneActorChase_QueryMotionFlags(actor, 0x20))
        return;
    state = actor->base.context;
    {
        u8 **players = &(*(u8 **)gPlayers);
        u8 **slots;
        slots = players;
        Chase_Delta(&delta, &state->previousPlayer, (void *)&gPlayers[0]->position);
    }
    state->previousPlayer = *(struct ContactPosition *)&gPlayers[0]->position;
    if (state->reactionDelay) {
        state->reactionDelay--;
        return;
    }
    if (SceneActorChase_QueryMotionFlags(actor, 0x100))
        return;
    if (!delta.x && !delta.z)
        return;
    reaction = (void *)&actor->base.tracking.target;
    if (!SceneActorStimulus_TestMotion(reaction, 2))
        return;
    {
        u32 mask = ~2;
        reaction->flags = mask & reaction->flags;
    }
    height = actor->base.body.position.y - gPlayers[0]->position.y;
    if (height < 0)
        height = -height;
    if (height > 512)
        return;
    if (actor->base.tracking.distance >= actor->base.tracking.limit)
        return;
    active = Ghost_GetReactionFlags(state, 0x10, &savedFlags);
    if (active) {
        if (gPlayers[0]->currentStatB == 0)
            return;
        SceneActorTerrain_EnterState(actor, 0, 0, 4);
        state->delay = state->delayDuration;
    } else {
        WordFlags_Set(0x10, savedFlags);
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

void Ghost_SetAfterimagePaletteWord(struct ChaseActor *actor, u32 value)
{
    struct Ghost *g = actor->base.context;
    s32 i;
    for (i = 3; i >= 0; i--)
        g->visuals[i].sequence.animation.paletteWord = value;
}

void SceneActor_InitLinkedSprites(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
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

void Ghost_EmitAfterimage(struct ChaseActor *actor)
{
    struct Ghost *ctx = actor->base.context;
    u32 variant;
    u32 flags;
    if (Mod(actor->base.frameCounter, ctx->visualInterval) == 0) {
        Ghost_GetDirectionalFrame(actor->base.body.direction, &variant, &flags);
        {
            struct ActorAnim *source = &((struct GhostVisual *)actor->base.drawable)->sequence.anim;
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
            *position = actor->base.body.position;
        }
        ctx->visuals[ctx->visualIndex].sprite.flags &= ~1;
        ctx->visuals[ctx->visualIndex].sequence.life = 8;
        if ((++ctx->visualIndex) > 3)
            ctx->visualIndex = 0;
    }
}

void ActorContact_TickAuxiliaryTimers(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
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

static inline void Ghost_InterpolatePoint(struct ContactPosition *out,
                                          struct ContactPosition *start,
                                          struct ContactPosition *end, s32 tx, s32 tz, s32 t)
{
    struct ContactPosition offset;
    struct ContactPosition acc;
    s32 q, p, twiceP, thriceQ, tangent, h11;
    s32 scaled, value;
    Vanargand_Difference(&offset, end, start);
    ZeroFill8(&acc);
    q = MovementCurve_TruncateFixed10(t * t);
    p = MovementCurve_TruncateFixed10(q * t);
    twiceP = p * 2;
    thriceQ = q * 3;
    tangent = (p - 2 * q + t) * tz;
    h11 = p - q;
    scaled = ((twiceP - thriceQ + 1) * acc.x) * tx;
    value = (thriceQ - twiceP) * offset.x;
    value = scaled + value + tangent + h11;
    out->x = start->x + MovementCurve_TruncateFixed10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.y) * tx;
    value = (-twiceP + thriceQ) * offset.y;
    value = scaled + value + tangent + h11;
    out->y = start->y + MovementCurve_TruncateFixed10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.z) * tx;
    value = (-twiceP + thriceQ) * offset.z;
    value = scaled + value + tangent + h11;
    out->z = start->z + MovementCurve_TruncateFixed10(value);
}

void Ghost_BuildMovementCurve(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Ghost *route = actor->base.context;
    struct ContactPosition start;
    struct ContactPosition end;
    s32 step, tx, tz, i;

    start = actor->base.body.position;
    end = *target;
    start.y = 0;
    end.y = 0;
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

static inline void Hide(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    {
        u32 *flags = actor->base.drawable;
        u32 value = *flags;
        *flags = value | 1;
    }
    {

        ActorShadow_Disable(actor->base.shadow);
    }
}

static inline void Show(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    {
        u32 *flags = actor->base.drawable;
        u32 value = *flags;
        *flags = value & ~1;
    }
    {

        ActorShadow_Enable(actor->base.shadow);
    }
}

// confidence=low: flickers the drawable and shadow during a pending visibility transition.

void Ghost_TickVisibilityTransition(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct Ghost *state;
    u32 unavailable = SceneActor_ActionsBlocked(actor);
    if (unavailable)
        return;
    {

        u32 mask;
        u32 active;
        state = actor->base.context;
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
                    u32 *field = actor->base.drawable;
                    u32 value = *field;
                    u32 mask = ~1;
                    *field = value & mask;
                }
                {
                    u32 mask = ~0x20;
                    *flags &= mask;
                }
                ActorShadow_Enable(actor->base.shadow);
            } else {
                {
                    u32 *field = actor->base.drawable;
                    u32 value = *field;
                    *field = value | 1;
                }
                {
                    u32 mask = ~0x40;
                    *flags &= mask;
                }
                ActorShadow_Disable(actor->base.shadow);
            }
        }
    }
    {
        u16 *counter = &state->visibilityTimer;
        (*counter)++;
    }
}

void ActorVisibility_TickRevealDelay(struct ChaseActor *actor)
{
    struct Ghost *s = actor->base.context;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        DEBUG_PRINT();
    }
    if (s->revealTicks) {
        *(u32 *)actor->base.drawable &= ~1;
        s->revealTicks--;
    }
}

void Ghost_EndPlayerSlowdown(struct ChaseActor *actor)
{
    struct Ghost *q = actor->base.context;

    if (Ghost_TestFlags(q, 0x80)) {
        u8 *p = &gPlayers[0]->movementSpeedReduction;
        *p = *p - 3;
        {
            u32 *f = &((struct Ghost *)actor->base.context)->flags;
            u32 m = ~0x80;
            *f = *f & m;
        }
    }
}

void ActorTick_DecrementCounterAA(struct ChaseActor *actor)
{
    if (!SceneActor_ActionsBlocked(actor)) {
        s16 *p = (s16 *)&actor->base.attachmentTimer;
        s32 v = *p;
        if (v > 0)
            *p = v - 1;
    }
}

static inline u32 Ghost_ReadMotionFlags(struct ChaseActor *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u16 *flags = &actor->base.motionFlags;
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

static inline u32 Ghost_ReadPlayerRevealFlag(struct Player *p)
{
    return WordFlags_Test(&p->actionFlags, 0x10);
}

u32 Ghost_UpdateVisibility(struct ChaseActor *actor)
{
    struct ChaseActor *saved = actor;
    struct Ghost *ctx;
    {
        struct ContactCollider *c = &actor->base.colliderB;
        c->flags |= 4;
    }
    if (Ghost_ReadMotionFlags(actor, 0x4000)) {
        if (Ghost_TestFlags((void *)actor, 0x200)) {
            *((u32 *)actor->base.drawable) |= 1;
            ActorShadow_Disable(actor->base.shadow);
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
    ctx = actor->base.context;
    if (!SceneActor_ActionsBlocked(actor)) {
        s32 reveal;
        if (SceneActorChase_Test(gEngineFlags, 0x4800))
            DEBUG_PRINT();
        if (Ghost_TestFlags(ctx, 0x100)) {
            reveal = GameState_TestWorldFlags(0x20);
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
            if (!Ghost_TestFlags(ctx, 0x40)) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (!hidden) {
                    ctx->visibilityTimer = hidden;
                    Ghost_ClearFlags(ctx, 0x20);
                    Ghost_SetFlags(ctx, 0x40);
                }
            }
        } else {
        hide:
            if (!Ghost_TestFlags(ctx, 0x20)) {
                if ((u8)SceneActorVisibility_IsHidden(actor)) {
                    ctx->visibilityTimer = 0;
                    Ghost_ClearFlags(ctx, 0x40);
                    Ghost_SetFlags(ctx, 0x20);
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

u8 SceneActorBehavior_IsNearParticipant(struct ChaseActor *);

u32 ActorContact_UpdateColliderPositions(struct RoomActor *actor)
{
    u16 *control;
    struct ContactPosition *position;
    ActorContact_TickAuxiliaryTimers(actor);
    {
        u32 mask = 0x20;

        control = &actor->motionFlags;
        if (*control & mask)
            goto done;
    }
    if (SceneActorTurn_Flags(control, 0x110))
        goto secondary;
    {
        u32 near = (u32)SceneActorBehavior_IsNearParticipant((void *)actor) << 24;
        position = (void *)actor;
        position = &((struct RoomActor *)position)->body.position;
        if (!near || SceneActorTurn_Flags(control, 6))
            goto primary;
    }
secondary: {
    struct ContactCollider *collider = &actor->colliderB;
    u32 flags = collider->flags;
    u32 disabled = 4;
    disabled &= flags;
    position = (void *)actor;
    position = &((struct RoomActor *)position)->body.position;
    if (!disabled) {
        Collider_SetPosition(collider, position, 0);
        Collider_Refresh(collider);
    }
}
primary:
    Collider_SetPosition(&actor->colliderA, position, 0);
done:
    return 1;
}

static inline void Ghost_UpdateAnimationScale(struct ChaseActor *actor, u16 *flags)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(&((struct GhostVisual *)actor->base.drawable)->sequence.anim, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

static inline void Ghost_SavePlayerPosition(struct ContactPosition *out, struct Player **players)
{
    struct Player **source = players;
    *out = *(struct ContactPosition *)&(*source)->position;
}

/* Follow the drained player's displacement until the drain finishes. */

static inline u16 *Ghost_BeginPlayerFollow(struct ChaseActor *actor, struct Ghost *context)
{
    u8 taken = SceneActorControl_TakeFlag(&actor->base.field2B1);
    if (taken) {
        SceneActorChase_SetControl(actor, Ghost_UpdateEnergyDrainAnimation, 0x23);
        Ghost_SavePlayerPosition(&context->start, gPlayers);
        SceneActorChase_OrHalfFlags(&actor->base.motionFlags, 0x100);
        SceneActorChase_SetActorFlags(actor, 0x2000000);
        actor->base.body.direction = actor->base.tracking.heading;
    }
    return &actor->base.motionFlags;
}

void Ghost_FollowDrainedPlayer(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
    struct ContactPosition delta;
    u16 *flags = Ghost_BeginPlayerFollow(actor, context);
    {
        u32 scale = 64;
        actor->base.paletteTimer = scale;
        Chase_Delta(&delta, &context->start,
                                (struct ContactPosition *)&gPlayers[0]->position);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        context->start = *((struct ContactPosition *)&gPlayers[0]->position);
        Ghost_UpdateAnimationScale(actor, flags);
    }
    if (actor->base.stepDone || (gPlayers[0]->currentStatB == 0)) {
        Sound_PlaySong(0x3CC);
        Chase_SetBehavior(actor, 3);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~4);
    } else {
        ++actor->base.counter2AC;
    }
}

static inline void Ghost_MakeLoopMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = SceneRoute_ScaleComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = SceneRoute_ScaleComponent(gUnk_086149C4[angle & 255] * speed);
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

void Ghost_FollowPlayerLoop(struct ChaseActor *actor)
{
    struct Ghost *route = actor->base.context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = Ghost_FirstLoopRandom(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        Ghost_MakeLoopMotion(
            &route->motion,
            Ghost_OffsetLoopHeading(
                ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                &actor->base.tracking.heading) &
                255,
            32);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed = 1;
    }
    Chase_Delta(&delta, &actor->base.body.position, &route->points[route->index]);
    Ghost_EmitAfterimage(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->base.body.direction =
            Math_Atan2(route->points[route->index].x - actor->base.body.position.x,
                       route->points[route->index].z - actor->base.body.position.z);
        Ghost_MakeLoopMotion(&delta, actor->base.body.direction, route->routeSpeed);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        route->motion = actor->base.body.motion;
    } else if (++route->index > 8) {
        Chase_Delta(&route->motion, &actor->base.body.position, &route->points[8]);
        route->motion.y = 0;
        actor->base.body.position.x = route->points[8].x;
        actor->base.body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        Ghost_MakeLoopMotion(
            &delta,
            Ghost_OffsetLoopHeading(Ghost_NextLoopRandom(), &actor->base.tracking.heading) & 255,
            512);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed >>= 1;
        return;
    }
    if (actor->base.paletteOffset == 0) {
        if (Mod(actor->base.counter2AC, 16) == 0 && route->routeSpeed <= 11)
            ++route->routeSpeed;
    } else {
        if (Mod(actor->base.counter2AC, 8) == 0 && route->routeSpeed <= 11)
            route->routeSpeed += 2;
    }
    ++actor->base.counter2AC;
}

// confidence=low: follows nine stored route points and retargets around the player; action role tentative.

void Ghost_PursuePlayerLoop(struct ChaseActor *actor)
{
    struct Ghost *route = actor->base.context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = Ghost_FirstLoopRandom(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        Ghost_MakeLoopMotion(
            &route->motion,
            Ghost_OffsetLoopHeading(
                ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                &actor->base.tracking.heading) &
                255,
            32);
        Ghost_BuildMovementCurve(actor, &target);
        route->routeSpeed = 1;
    }
    Chase_Delta(&delta, &actor->base.body.position, &route->points[route->index]);
    Ghost_EmitAfterimage(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->base.body.direction =
            Math_Atan2(route->points[route->index].x - actor->base.body.position.x,
                       route->points[route->index].z - actor->base.body.position.z);
        Ghost_MakeLoopMotion(&delta, actor->base.body.direction, route->routeSpeed);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        route->motion = actor->base.body.motion;
    } else if (++route->index > 8) {
        Chase_Delta(&route->motion, &actor->base.body.position, &route->points[8]);
        route->motion.y = 0;
        actor->base.body.position.x = route->points[8].x;
        actor->base.body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct SceneActorChasePlayer **)gPlayers)->target.position;
        Ghost_MakeLoopMotion(
            &delta,
            Ghost_OffsetLoopHeading(Ghost_NextLoopRandom(), &actor->base.tracking.heading) & 255,
            256);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        Ghost_BuildMovementCurve(actor, &target);
        return;
    }
    if (Mod(actor->base.counter2AC, 16) == 0 && route->routeSpeed <= 17)
        ++route->routeSpeed;
    ++actor->base.counter2AC;
}

extern const u32 gUnk_08605DE8[];

void Ghost_InitActionTable(struct ChaseActor *actor)
{
    actor->base.behaviorCallbacks = gUnk_08605DE8;
}

void Ghost_ApproachPlayer(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, Ghost_UpdateWaitingAnimation, 10);
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        state->delay = state->delayDuration;
    }
    if (actor->base.stepDone)
        Ghost_StartMovementToPosition(actor, &gPlayers[0]->position);
    else
        actor->base.counter2AC++;
}

void Ghost_ChooseMovementDestination(struct ChaseActor *actor)
{
    struct Ghost *state = actor->base.context;
    struct ContactPosition destination;
    struct ContactPosition *target, *endpoint;

    SceneActorControl_TakeFlag(&actor->base.field2B1);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if ((s32)actor->base.counter2AC > 60) {
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
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        {
            struct ContactPosition *position = &actor->base.body.position;
            target = &destination;
            target->x -= position->x;
            endpoint->y -= position->y;
            endpoint->z -= position->z;
        }

        actor->base.nextDirection = Math_Atan2(target->x, endpoint->z);
        ZeroFill8(&state->motion);
        return;
    }
    actor->base.counter2AC++;
}

extern u8 gUnk_08605DF4[];

void Ghost_InitReactionTable(struct ChaseActor *actor)
{
    actor->base.alternateBehaviorCallbacks = gUnk_08605DF4;
}

extern u32 gUnk_02000560;

void Ghost_UpdateFacingTrigger(struct ChaseActor *actor)
{
    struct Ghost *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    {
        u32 mask = 0x800;
        actor->base.placementFlags = mask | actor->base.placementFlags;
    }
    actor->base.body.direction = actor->base.tracking.heading;
    if (!(gPadState[1] & A_BUTTON))
        goto wait;
    if (((u8 **)gPlayers)[0]) {
        s32 own = ((actor->base.body.direction + 0x20) & 0xFF) >> 5;
        s32 player = (((u8)gPlayers[0]->unk2D + 0x20) & 0xFF) >> 5;
        s32 diff = ((own + 4) & 7) - player;
        if (diff < 0)
            diff = -diff;
        if (diff <= 1 && actor->base.tracking.distance < context->triggerRange &&
            !(gEngineFlags & 0x200) && !Flags_MaskedWord(4, &gUnk_02000560)) {
            SceneActorChase_SetState(actor, 2, 0, 0);
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
            Script_Call(context->effectScript, 0);
            return;
        }
    }
wait:
    actor->base.counter2AC++;
}

void Ghost_UpdateTriggerControl(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    } else {
        MonsterInit_SetMotionFlags(actor, 0x800);
        if (actor->base.counter2AC == 0)
            Trigger_ConsumePending(actor->base.receiver, 1);
        actor->base.counter2AC++;
    }
}

extern u8 gUnk_08605DFC[];

void Ghost_InitEffectTable(struct ChaseActor *actor)
{
    actor->base.callbacks29C = gUnk_08605DFC;
}

void SceneActorControl_DispatchControl(u8 *);

u32 Ghost_OnBehaviorUpdate(struct ChaseActor *actor)
{
    u32 i;
    void **t;
    Ghost_UpdateVisibility(actor);
    i = actor->base.state2A8;
    t = actor->base.stateCallbacks;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    ActorContact_UpdateColliderPositions(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Ghost_OnActorUpdate(struct ChaseActor *actor)
{
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
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

u32 Ghost_Destroy(struct ChaseActor *actor)
{
    u8 *p;
    u8 *q;
    u32 m;
    u32 v;
    Ghost_RemoveAuxiliarySprites(actor);
    p = actor->base.drawable;
    if (actor->base.drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(&((struct SceneActorAnimationTileDrawable *)p)->tile);
    Heap_Free(p);
    m = 0x100;
    if (SceneActorChase_TestField(&actor->base.placementFlags, m))
        SceneMessages_UnregisterReceiver(actor->base.receiver);
    {
        u8 *r = actor->base.shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = ((struct ChaseActor *)actor)->base.statusFlags & m;
    q = (void *)&actor->base.body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&actor->base.colliderA);
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

void SceneActorScript_OnCommand(void *actor, const u16 *command);

extern const u8 gUnk_08605E14[];

static inline u8 Ghost_GetAnimationIndex(struct ChaseActor *actor)
{
    return actor->base.animationIndex;
}

static inline u8 Ghost_GetAnimationRestart(struct ChaseActor *actor)
{
    return actor->base.restart;
}

static inline void Ghost_InitializeScriptFields(struct ChaseActor *actor)
{
    struct SceneActorBody *body = &actor->base.body;
    u32 id;
    struct ContactPosition origin;
    s32 first, second;
    s32 variant;
    actor->base.behaviorFlags = 0;
    actor->base.statusFlags = 0;
    actor->base.enabled = 0;
    actor->base.field2E8 = 8;
    actor->base.placementFlags = MonsterInit_ReadKeywordHalfword('M', 0);
    ZeroFill8(&origin);
    id = Script_GetKeywordValue('N', 0);
    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(actor)->base.tracking);
    if (Script_SeekToKeyword('l')) {
        first = MonsterInit_ReadWord(300);
        second = MonsterInit_ReadWord(300);
    } else {
        first = 0;
        second = 0;
    }
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(actor, 0x200);
    actor->base.script1F0 = MonsterInit_ReadKeywordHalfword('F', 0);
    actor->base.scriptId = MonsterInit_ReadKeywordHalfword('R', 0);
    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(actor, MonsterInit_ReadHalfword(0));
        actor->base.field9E = MonsterInit_ReadByte(0);
        actor->base.defense = MonsterInit_ReadByte(0);
        actor->base.contactReward = MonsterInit_ReadByte(0);
        actor->base.fieldA2 = MonsterInit_ReadHalfword(0);
        actor->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }
    MonsterInit_InitializeHealth(actor, MonsterInit_GetInitialHealth(actor), first, second);
    if (Script_SeekToKeyword('X')) {
        actor->base.experience = MonsterInit_ReadWord(0);
        actor->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    MonsterInit_WriteHalfword(&actor->base.hudKind, MonsterInit_ReadKeywordHalfword('K', 2));
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    MonsterInit_WriteHalfword(&actor->base.hudLabel, variant);
    if (Script_GetKeywordValue('H', 0))
        MonsterInit_SetStatusFlags(actor, 0x800);
    SceneBody_Init(body, id, &origin, 0, 4, actor);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(actor) ? 0 : actor->base.drawable);
    SceneBody_SetMovementProbe(body, ((struct Ghost *)actor)->base.movementProbe, 0x40, 0x40);
}

static inline void Ghost_InstallPrimaryCallbacks(struct ChaseActor *actor, void *cb274,
                                                 u32 (*cb260)(void *), void *cb224, void *cb228,
                                                 void *cb22C, void *control, void *cb230,
                                                 void *cb234, void *defaultReaction,
                                                 void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.onUpdate = cb274;
    MonsterInit_WriteAnimationQuery(&actor->base.animationQuery.word, cb260);
    actor->base.onEntry = cb224;
    actor->base.onAlternateEntry = cb228;
    actor->base.onStanding = cb22C;
    actor->base.onReaction = cb230;
    actor->base.onDefeat = cb234;
    actor->base.onHidden = control;
    actor->base.defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, 0);
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, 0);
    actor->base.routeReset = 0;
    actor->base.targetQuery = targetQuery;
    actor->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    actor->base.onDamage = damage;
    actor->base.onContact = contact;
    actor->base.field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Ghost_InitializeCallbacks(struct ChaseActor *actor)
{
    SceneActor_InitLinkedReactionColliders(actor);
    Ghost_InstallPrimaryCallbacks(
        actor, Ghost_OnBehaviorUpdate, Ghost_GetBehaviorCode, Ghost_TurnTowardHeading,
        Ghost_UpdateDirectionalPose, Ghost_UpdateDirectionalPoseB, 0, Ghost_UpdateHitStagger,
        Ghost_UpdateDefeat, SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    actor->base.routeReset = Ghost_ResetMovementState;
    actor->base.reactionGate = Ghost_OnControlCleanup;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, Ghost_GetControlStatus);
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, gUnk_08605E14);
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
        actor->base.regionId = gGameState->region;
    else
        actor->base.regionId = Region_FindContaining(&actor->base.body.position);
    if (MonsterInit_TestMotionFlags(actor, 0x40)) {
        struct ContactCollider *collider = &actor->base.colliderA;
        u32 mask = ~0x100;
        collider->flags = mask & collider->flags;
    }
}

static inline void Ghost_ReadPalette(struct ChaseActor *actor, u32 base, u32 alternate)
{
    s32 selected;
    actor->base.paletteOffset = MonsterInit_ReadKeywordHalfword('C', 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    actor->base.alternateOffset = MonsterInit_GetPaletteOffset(actor) + base;
    selected = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (!MonsterInit_GetDrawableKind(actor))
        AnimData_SelectPaletteBank(
            &((struct GhostVisual *)actor->base.drawable)->sequence.animation, selected);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->palette =
            gPaletteBankData + (selected << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
            SceneActorScript_GetManagerField62());
    actor->base.alternatePalette = MonsterInit_GetPaletteBase(actor);
    actor->base.paletteBank = MonsterInit_GetPaletteBase(actor) + 2;
    actor->base.paletteTimer = 0x1EE;
}

static inline void Ghost_InstallCompletion(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Ghost_OnActorUpdate;
    actor->base.destroy = (SceneActorResultCallback)Ghost_Destroy;
}

static inline void Ghost_SelectAnimation(struct ChaseActor *actor, u32 selection, u32 variant,
                                         u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->base.drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->base.animationIndex = selection;
        drawable = (void *)actor->base.drawable;
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
        selection += variant;
        if (!force && Ghost_GetAnimationIndex(actor) == selection &&
            !Ghost_GetAnimationRestart(actor)) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        SceneActorTurn_SetAnimationIndex(actor, selection);
        {
            u32 mode;
            if (flags & 4)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                actor->base.drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= 4;
            else
                tile->flags &= ~4;
        }
    }
    actor->base.restart = 0;
}

static inline void Ghost_InitializeDrawable(struct ChaseActor *actor)
{
    struct ContactPosition position;
    if (!MonsterInit_GetDrawableKind(actor)) {
        u8 *d;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        d = MonsterInit_Drawable(actor);
        AnimData_Load(&((struct GhostVisual *)d)->sequence.animation, 0xD637);
        Sprite_Init(d, &((struct GhostVisual *)d)->sequence.animation, 0);
        SceneActorAnimation_InitEnemySequence(d, 0x29FB);
        if (actor->base.category == 0x11 || actor->base.category == 0x17)
            ((struct ActorSprite *)d)->priority = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *d;
        void *res;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        d = MonsterInit_Drawable(actor);
        ZeroFill8(&position);
        res = ScriptProc_Start(0xCB05, 0xD637);
        *(struct DisplayResourceHeader *)d = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(d, res);
        Widget_InitLinked(&((struct SceneActorAnimationTileDrawable *)d)->tile, d, 0, 0, 2, 2, 0x3C,
                          &position);
        actor->base.animationIndex = 0xFF;
    }
}

static inline u32 Ghost_ReadRoute(struct ChaseActor *actor, struct SceneActorRegistryRoute *route)
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
        route->branches[MonsterInit_RouteActiveBranch(route)] = 0;
        route->branches[MonsterInit_RouteAlternateBranch(route)] = 0;
    }
    return 0;
}

s32 Ghost_Init(void *object)
{
    struct ChaseActor *a = object;
    struct Ghost *arg0 = object;
    u32 row, frameFlags;
    struct SceneActorRegistryRoute *route;
    struct SceneActorProfileEntry *profile;
    arg0->flags = 0;
    SceneActorRegistry_SetObjectContext(a, a, 0, 0);
    {
        u8 category = MONSTER_CATEGORY_GHOST;
        a->base.category = category;
    }
    Ghost_InitActorProfile(a);
    Ghost_InitializeDrawable(a);
    Ghost_InitializeScriptFields(a);
    route = &arg0->route;
    if (Ghost_ReadRoute(a, route)) {
        {
            struct ContactPosition *dest = &a->base.body.position;
            struct SceneActorRegistryRoute *link = route;
            dest->x = MonsterInit_RoutePointXRaw(link);
            dest->z = MonsterInit_RoutePointZRaw(link);
            dest->y = MonsterInit_TerrainHeight(MonsterInit_CellIndex(dest), dest);
        }
        MonsterInit_DrawablePosition(a);
        a->base.body.direction = Route_GetCurrentPointFacing(route);
    }
    if (Script_SeekToKeyword('p')) {
        a->base.body.position.x = MonsterInit_ReadHalfword(0);
        a->base.body.position.y = MonsterInit_ReadHalfword(0);
        a->base.body.position.z = MonsterInit_ReadHalfword(0);
        a->base.terrainCell = MonsterInit_CellIndex(&a->base.body.position);
        a->base.groundHeight =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(a), &a->base.body.position);
        arg0->baseHeight = a->base.groundHeight;
        MonsterInit_DrawablePosition(a);
    } else
        arg0->baseHeight = a->base.groundHeight;
    arg0->delayDuration = MonsterInit_ReadKeywordHalfword('l', 0);
    arg0->previousPlayer = *(struct ContactPosition *)&gPlayers[0]->position;
    if (Script_SeekToKeyword('P')) {
        arg0->drainInterval = MonsterInit_ReadByte(0);
        arg0->drainDuration = MonsterInit_ReadByte(0);
        arg0->particleStyle = MonsterInit_ReadByte(0);
    }
    if (Script_GetKeywordValue('g', 0))
        Ghost_SetFlags(arg0, 0x100);
    if (Script_SeekToKeyword('d')) {
        arg0->effectScript = MonsterInit_ReadHalfword(0);
        MonsterInit_ReadScriptWord(&arg0->triggerRange, 0);
        arg0->triggerRange *= arg0->triggerRange;
    } else {
        arg0->effectScript = 0;
        arg0->triggerRange = 0;
    }
    if (Script_GetKeywordValue('i', 0))
        Ghost_SetFlags(arg0, 0x200);
    Ghost_ReadPalette(a, 0x1EB, 0x192);
    if (!Script_GetKeywordValue('U', 0)) {
        struct Ghost *context = a->base.context;
        s32 enabled;
        if (Ghost_TestFlags(context, 0x100))
            enabled = MonsterInit_TestWorldFlags(0x20);
        else
            enabled = gGameState->unk876;
        if (enabled > 0) {
            Ghost_SetFlags(context, 0x40);
            ((struct ActorSprite *)a->base.drawable)->flags |= 1;
            context->visibilityTimer = 0;
        } else {
            Ghost_SetFlags(context, 0x20);
            ((struct ActorSprite *)a->base.drawable)->flags &= ~1;
            context->visibilityTimer = 0;
        }
    }
    Ghost_InitializeCallbacks(a);
    if (Script_GetKeywordValue('S', 0) || !MonsterInit_IsInCurrentRegion(a))
        SceneActorPlacement_HideActor(a, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
    if (!MonsterInit_GetDrawableKind(a))
        DisplaySequence_Step(a->base.drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile,
                  a->base.drawable);
    MonsterInit_InitializeReceiver(a);
    SceneActorBehavior_InstallCallbackTables(a);
    Ghost_InitMotionPreset(a);
    Ghost_InitActionTable(a);
    Ghost_InitReactionTable(a);
    if (arg0->effectScript) {
        Ghost_InitEffectTable(a);
        MonsterInit_SetStatusFlags(a, 8);
    }
    profile = (void *)SceneActorProfiles_GetEntryAddress(a->base.profileId, a->base.profileMode, 1);
    Ghost_GetDirectionalFrame(a->base.body.direction, &row, &frameFlags);
    Ghost_SelectAnimation(a, profile->value, row, frameFlags);
    a->base.field2E8 = 2;
    SceneActor_InitLinkedSprites(a);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(a), a);
    a->base.saved.privateFlags = arg0->flags;
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
