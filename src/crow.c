#include "crow.h"
#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_registry.h"
#include "scene_actor_animation.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_turn.h"
#include "scene_actor_behavior.h"
#include "script_utility.h"
#include "ram.h"
#include "player.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "scene_actor_control_motion.h"
#include "display_sequence_inline.h"
#include "display_animation.h"
#include "scene_actor_script.h"
#include "engine.h"
#include "scene_actor_route.h"
#include "scene_actor_state.h"
#include "scene_body_api.h"
#include "render.h"
#include "engine_globals.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_dispatch.h"

/* Records type-one contacts reported by the Crow attack collider. */

void Crow_OnAttackContact(u8 *unused, struct ContactCollider *obj,
                          struct SceneActorRegistryResetView *ctx)
{
    int mask;
    u32 *flags;
    if (obj->contactFlags != 1)
        return;
    if ((SceneActor_ActionsBlocked(ctx) == 0) && (obj->contactFlags & 0xF)) {
        ctx->field304 = obj->collisionId;
        flags = &ctx->flags2D0;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);

void Crow_OnBodyContact(struct SceneActorBehaviorContactView *source,
                        struct SceneActorBehaviorContactView *destination,
                        struct SceneActorRegistryResetView *actor)
{
    u32 index;
    s32 power;
    u32 sample;
    if (SceneActor_ActionsBlocked((u8 *)actor)) {
        return;
    }
    {
        u32 mask = 0x800;
        if (source->flags & mask) {
            return;
        }
    }
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    {
        const u16 *table = (const u16 *)0x0203B400;
        sample = *(u16 *)(index + (u32)table);
    }
    {
        s32 old = destination->power;
        u32 mask = 15;
        power = old - (sample & mask);
    }
    power = power < 20 ? 20 : power;
    destination->power = power;
}

void Crow_OnIdleAnimation(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry(actor, 1);
    Crow_PlayAnimationCue(actor);
}

s32 Mod(s32, s32);

static inline u8 Crow_TestAnimationFlags(struct Crow *state, u32 mask)
{
    if (state->flags & mask)
        return 1;
    return 0;
}

void Crow_UpdatePeriodicAnimation(struct SceneActorRegistryActor *actor, s32 tick)
{
    struct Crow *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    } else if (!Crow_TestAnimationFlags(state, 4)) {
        if (!Mod(tick, 90)) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 15);
            state->wanderTicks += 40;
        } else if (actor->field2B3) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        }
    }
}

void Crow_UpdateAnimationSequence(struct SceneActorRegistryActor *actor)
{
    struct Crow *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 16);
        state->sequencePhase = 0;
    }
    if (state->sequencePhase)
        Crow_PlayAnimationCue((u8 *)actor);
    if (actor->field2B3) {
        switch (state->sequencePhase) {
            case 0:
            case 1:
            case 2:
            case 3:
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 14);
                break;
            case 5:
                actor->field2B0 = 1;
                return;
        }
        state->sequencePhase++;
    }
}

// confidence=low: plays profile entry two when the action begins.

void Crow_PlayEntry2(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorTurn_SelectProfileAnimation((void *)actor, 2, 0, 0);
}

static inline void Crow_SelectAnimation(struct SceneActorAnimationView *actor, u32 selection,
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

void Crow_UpdateDirectionalPose(struct SceneActorAnimationView *actor)
{
    struct SceneActorProfileEntry *entry;
    struct SceneActorBody *body;
    s32 variant, flags;
    u32 selection;
    if (!SceneActorControl_TakeFlag(&((struct SceneActorRegistryResetView *)actor)->field2B2))
        return;
    body = &((struct SceneActorRegistryResetView *)actor)->body;
    entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 2);
    Crow_GetDirectionalFrameAndFlip(body->direction, &variant, &flags);
    selection = entry->value;
    Crow_SelectAnimation(actor, selection, variant, flags);
}

void Crow_OnSpecialAnimation(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0xe);
}

static inline s32 Crow_QuarterMotion(s32 n)
{
    if (n >= 0)
        n = n >> 2;
    else
        n = -((-n) >> 2);
    return n;
}

static inline u8 Crow_EaseReactionMotion(struct SceneActorRegistryResetView *actor)
{
    s32 x = Crow_QuarterMotion(actor->reactionStart.x - actor->reactionEnd.x);
    s32 z = Crow_QuarterMotion(actor->reactionStart.z - actor->reactionEnd.z);
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    actor->reactionEnd.x += x;
    actor->reactionEnd.z += z;
    if (x == 0 && z == 0)
        return 1;
    return 0;
}

static inline void Crow_SetRecoveryState(struct SceneActorRegistryResetView *actor, u32 state,
                                         u32 phase, u8 behavior, u32 zw)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = zw;
    actor->field2B1 = 1;
}

static inline void Crow_SetRecoveryControl(struct SceneActorRegistryResetView *actor,
                                           void *callback, u32 kind, u32 zb, u32 zw)
{
    actor->field2B2 = 1;
    actor->field2B0 = zb;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = zw;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zw;
}

// confidence=low: eases the stored reaction displacement to its target before restoring idle.

void Crow_RecoverFromDisplacement(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    struct Crow *state = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u32 m = 0x20;
        m |= actor->flags2D4;
        actor->flags2D4 = m;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
    }
    {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
    }
    if (elapsed < 20) {
        u32 m = 0x20;
        m |= actor->flags2D6;
        actor->flags2D6 = m;
    }
    if (Crow_EaseReactionMotion(actor) && elapsed > 30) {
        u32 clearedWord;
        u16 v = actor->colliderA.responseValue + 10;
        u16 initialState[] = {0};
        clearedWord = 0;

        actor->colliderA.responseValue = v;
        actor->field2B0 = 1;
        {
            u16 *field = &actor->flags2D4;
            u32 mask;
            mask = ~0x20;
            ((struct CrowSignedFlags *)field)->bits &= mask;
        }
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x80;
            *flags &= mask;
        }
        state->bobPhase = initialState[0];
        Crow_DisableAttackCollision(actor);
        actor->body.direction = actor->tracking.heading;
        Crow_SetRecoveryControl(actor, SceneActorControl_OnIdle, 16, initialState[0], clearedWord);
        Crow_SetRecoveryState(actor, initialState[0], initialState[0], 1, clearedWord);
        actor->body.direction = actor->tracking.heading;
        actor->field2C2 = 64;
    }
}

/* Defeat lifecycle: blink, script and event completion, then idle. */

void *BestiaryDisplay_GetEntryForActor(u32, u32);

void GameState_SetEventFlag(void *);

void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);

void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

static inline void Crow_SetDefeatState(struct SceneActorRegistryResetView *actor, u8 state,
                                       u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Crow_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            u32 offset = 0x13A;
            u16 *flags = &actor->colliderA.flags;
            {
                u32 v = *flags;
                u32 m = 4;
                m |= v;
                *flags = m;
            }
        }
    }
    if (!Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30))
        return;
    actor->field2B0 = 1;
    script = (u16)((struct SceneActorBehaviorView *)actor)->script1F8;
    if (script) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[0];
        values[4] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[1];
        values[5] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[2];
        values[6] = actor->field2E0;
        values[7] = actor->body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        if (!(actor->flags2D4 & mask) && (u32)(actor->category - 11) > 2)
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    }
    {
        u32 mask = 0x400;
        if (!(actor->flags2CC & mask))
            SceneActorBehavior_CallTerrainScript((void *)actor);
    }
    actor->fieldAA = 0;
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind;
        kind = 16;
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
    }
    Crow_SetDefeatState(actor, 1, 0, 2);
    ActorShadow_Disable(actor->shadow);
}

// confidence=low: hops within the stored home terrain cell.

extern const s16 gUnk_086149C4[];

static inline u32 Crow_NextHopRandom(const u16 *table)
{
    u32 i = (gRandomIndex + 1) & 1023;
    gRandomIndex = i;
    i *= 2;
    return *(const u16 *)((u32)table + i);
}

static inline s32 Crow_ScaleFixed12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void Crow_MakeHopVelocity(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Crow_ScaleFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Crow_ScaleFixed12(gUnk_086149C4[angle & 255] * speed);
}

static inline void Crow_AddHopPosition(struct ContactPosition *out, const struct ContactPosition *a,
                                       const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

static inline void Crow_CopyPositionWords(u32 control, const u32 *src, struct ContactPosition *dst)
{
    CpuSet(src, dst, control);
}

static inline void Crow_ClearHopVelocity(u32 *zero, struct ContactPosition *dst)
{
    *zero = 0;
    Crow_CopyPositionWords(0x05000002, zero, dst);
}

static inline void Crow_GetCellCenter(struct ContactPosition *p, s32 cell)
{
    s32 x = Mod(cell, ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width);
    s32 z = Div(cell, ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width);
    struct SceneActorChaseTerrainOverride *override;
    struct SceneActorChaseTerrainCell *data;
    p->x = (x << 8) + 128;
    override = (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        data = &override->properties;
    else
        data = &((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->cells[cell];
    p->y = (data->attributes & 15) << 8;
    p->z = (z << 8) + 128;
}

static inline u8 Crow_TakeHopEntry(struct SceneActorRegistryResetView *actor, u8 **saved)
{
    u8 *field = &actor->field2B1;
    u32 value = *field;
    *saved = field;
    if (value) {
        *field = 0;
        return 1;
    }
    return 0;
}

// confidence=low: random horizontal motion and gravity keep the hop inside its home cell.

void Crow_HopWithinHomeCell(struct SceneActorRegistryResetView *actor)
{
    struct Crow *state = actor->context;
    struct {
        u32 zero;
        struct ContactPosition home, next;
    } local;
    s32 ground = SceneActorChase_HeightAt(actor->terrainCell, &actor->body.position);
    u8 *arrived;
    s32 cell;
    if (Crow_TakeHopEntry(actor, &arrived)) {
        u32 angle;
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        Crow_GetCellCenter(&local.home, state->homeCell);
        actor->body.direction = angle = Crow_NextHopRandom(gRandomTable) & 255;
        Crow_ClearHopVelocity(&local.zero, &state->vectors[2]);
        Crow_MakeHopVelocity(&state->vectors[2], angle, (Crow_NextHopRandom(gRandomTable) & 7) + 1);
        state->vectors[2].y = 14;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    Crow_AddHopPosition(&local.next, &actor->body.position, &state->vectors[2]);
    cell = SceneActorChase_CellAt(&local.next);
    if (state->homeCell != cell) {
        state->vectors[2].x = 0;
        state->vectors[2].z = 0;
    }
    actor->body.motion.x += state->vectors[2].x;
    actor->body.motion.z += state->vectors[2].z;
    actor->field2EA += state->vectors[2].y;
    state->vectors[2].y -= 2;
    if (ground >= (s16)actor->field2EA) {
        actor->body.position.y = ground;
        actor->field2EA = ground;
        Crow_ClearHopVelocity(&local.zero, &state->vectors[2]);
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        *arrived = 1;
    } else
        actor->counter2AC++;
}

static inline s32 Crow_NormalizeHeading(s32 n)
{
    if (n > 128)
        n -= 256;
    else if (n < -128)
        n += 256;
    return n;
}

static inline s32 Crow_HeadingMagnitude(s32 n)
{
    if (n < 0)
        n = -n;
    return n;
}

static inline s32 Crow_TruncateShift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline void Crow_FindPositionDelta(struct ContactPosition *out,
                                          const struct ContactPosition *from,
                                          const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline void Crow_MakeSteeringVelocity(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Crow_TruncateShift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Crow_TruncateShift(gUnk_086149C4[angle & 255] * speed, 12);
}

static inline s32 Crow_ClampTurnSpeed(s32 n, s32 min, s32 *out)
{
    *out = n;
    n = n > 0 ? n : min;
    return n;
}

static inline void Crow_SetFlightState(struct SceneActorRegistryResetView *actor, u8 state,
                                       u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

// confidence=low: distance and angle gates suggest approach and orbit flight steering.

void Crow_SteerAroundPlayer(struct SceneActorRegistryResetView *actor)
{
    struct Crow *state = actor->context;
    struct ContactPosition delta;
    struct ContactPosition velocity;
    s32 angle;
    s32 turn;
    s32 step;
    s16 t1;
    s16 t2;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        ((struct ActorSprite *)actor->drawable)->flags |= 2;
        SceneActorChase_SetControl(actor, actor->entryTarget224, 2);
    }
    if (state->approachTime < ((s32)actor->counter2AC)) {
        Crow_FindPositionDelta(&delta, &actor->body.position, (void *)&gPlayers[0]->position);
        t1 = delta.x;
        t2 = delta.z;
        angle = Math_Atan2(t1, t2);
        turn = Crow_NormalizeHeading(angle - actor->body.direction);
        if (turn < 0)
            turn = -turn;
        if (turn <= 11) {
            state->approachTurns = 0;
            ((struct ActorSprite *)actor->drawable)->flags &= ~2;
            Crow_SetFlightState(actor, 0, 2, 0);
            return;
        }
        step = Crow_ClampTurnSpeed(Crow_TruncateShift(turn, 2), 4, &step);
        if (step > 8)
            step = 8;
        if (state->approachTurns > 20) {
            state->approachTurns = 0;
            state->turnDelay = 0;
            Crow_SetFlightState(actor, 0, 0, 4);
            return;
        } else {
            state->approachTurns++;
            state->turnDelay = 0;
        }
    } else if (actor->tracking.distance <= 0x7CFF) {
        Crow_SetFlightState(actor, 0, 0, 4);
        return;
    } else if (state->orbitRange > actor->tracking.distance) {
        Crow_FindPositionDelta(&delta, (void *)&gPlayers[0]->position, &actor->body.position);
        angle = Math_Atan2(delta.x, delta.z);
        turn = Crow_HeadingMagnitude(
            Crow_NormalizeHeading(((angle + 64) & 255) - actor->body.direction));
        if (turn <= 63)
            angle += 64;
        else
            angle += 192;
        angle &= 255;
        step = Div(state->speed, 8);
        if (step <= 0)
            step = 2;
        goto clamp;
    } else {
        Crow_FindPositionDelta(&delta, &actor->body.position, (void *)&gPlayers[0]->position);
        angle = Math_Atan2(delta.x, delta.z);
        turn = Crow_NormalizeHeading(angle - actor->body.direction);
        if (turn < 0)
            turn = -turn;
        if (turn <= 31) {
            Crow_SetFlightState(actor, 0, 0, 4);
            return;
        }
        step = Crow_ClampTurnSpeed(Crow_TruncateShift(turn, 3), 2, &step);
    clamp:
        if (step > 6)
            step = 6;
    }
    {
        u8 *speed = &state->speed;
        struct ContactPosition *out = &velocity;
        Crow_MakeSteeringVelocity(out, actor->body.direction, *speed);
    }
    actor->body.motion = velocity;
    if (!state->turnDelay) {
        s32 difference = Crow_NormalizeHeading(angle - actor->body.direction);
        if (difference > 0)
            actor->body.direction += step;
        else
            actor->body.direction -= step;
        ((struct ActorSprite *)actor->drawable)->unk04[2] = 0x9F - actor->body.direction;
        if (actor->paletteOffset == 0)
            state->turnDelay = 4;
        else
            state->turnDelay = 2;
    }
    if (state->turnDelay)
        state->turnDelay--;
    actor->counter2AC++;
}

static inline u32 Crow_NextWanderRandom(void)
{
    u32 *indexPtr = &gRandomIndex;
    u32 i = (*indexPtr + 1) & 1023;
    *indexPtr = i;
    i *= 2;
    {
        u32 tab = RANDOM_TABLE_ADDR;
        return *(const u16 *)(i + tab);
    }
}

static inline u8 Crow_TestFlags(u32 mask, u32 flags)
{
    if (flags & mask)
        return 1;
    return 0;
}

void Crow_UpdateWanderTimer(struct SceneActorRegistryResetView *actor)
{
    struct Crow *context = actor->context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u32 mask = 0x7F;
        u32 value;
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
        value = Crow_NextWanderRandom();
        context->wanderTicks = (mask & value) + 0x14;
    }
    {
        u32 four = 4;
        if (!Crow_TestFlags(four, context->flags)) {
            u8 result = Callback_InvokeActor(actor, actor->contactQuery.opaque);
            if (!result) {
                if ((s32)actor->counter2AC > context->wanderTicks && actor->field2B3) {
                    SceneActorChase_SetBehavior(actor, 2);
                } else {
                    actor->counter2AC++;
                }
            }
        }
    }
}

static inline u8 Crow_TakeReturnEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void Crow_SetReturnState(struct SceneActorRegistryResetView *actor, u32 phase,
                                       u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Crow_UpdateReturnAnimation(struct SceneActorRegistryResetView *actor)
{
    if (Crow_TakeReturnEntry(actor)) {
        SceneActorChase_SetControl(actor, Crow_UpdateAnimationSequence, 0x23);
    }
    Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (actor->field2B0) {
        Crow_SetReturnState(actor, 2, 1);
    } else {
        actor->counter2AC++;
    }
}

void Crow_ApproachPlayer(struct SceneActorRegistryResetView *actor)
{
    struct Crow *state = actor->context;
    struct ContactPosition delta, velocity;
    u8 **pp;
    s32 limit;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        *(u32 *)actor->drawable &= ~2;
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
    }
    limit = actor->paletteOffset == 1 ? 0x18 : 0x28;
    if ((s32)actor->counter2AC > limit) {
        if (state->orbitRange > actor->tracking.distance) {
            Crow_SetFlightState(actor, 0, 0, 1);
            return;
        }
        pp = &gPlayers;
        Crow_FindPositionDelta(&delta, &actor->body.position,
                               &((struct SceneActorChasePlayer *)*pp)->target.position);
        if (Crow_HeadingMagnitude(Crow_NormalizeHeading(Math_Atan2(delta.x, delta.z) -
                                                        actor->body.direction)) > 0x40) {
            Crow_SetFlightState(actor, 0, 0, 1);
            return;
        }
    }
    Crow_MakeSteeringVelocity(&velocity, actor->body.direction, state->speed);
    actor->body.motion = velocity;
    actor->counter2AC++;
}

extern u8 gUnk_08605E94[];

void Crow_InitActionTable(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = gUnk_08605E94;
}

void Crow_BeginScriptedAction(struct SceneActorRegistryResetView *actor)
{
    u32 zero = SceneActor_ActionsBlocked(actor);
    if (zero == 0 && (*(u8 **)gPlayers)) {
        struct Crow *p = actor->context;
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 three;
        taken = &actor->field2B2;
        one = 1;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = code;
        actor->target = fn;
        zero8 = 0;
        actor->field2C8 = zero;
        flags = &actor->flags114;
        {
            u32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
        three = 3;
        actor->state2A8 = zero8;
        actor->phase2A9 = zero8;
        actor->behavior2AA = three;
        actor->counter2AC = zero;
        actor->field2B1 = one;
        {
            u32 *otherflags = &p->flags;
            u32 mask = -5;
            *otherflags &= mask;
        }
        {
            struct SceneActorChaseTracking *extra = &actor->tracking;
            u32 mask = 8;
            mask |= extra->flags;
            extra->flags = mask;
        }
        {
            u32 mask = 0x100000;
            actor->flags2D0 |= mask;
        }
    }
}

/* Crow ground-state reset (tentative): clears the context's state, restores the stored position,
 * resnaps the actor to the terrain height and steps the display sequence. */

static inline void Crow_FillResetPosition(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

void Crow_ResetGroundState(struct SceneActorRegistryResetView *actor)
{
    struct Crow *context = actor->context;
    u32 zero;
    context->sequencePhase = 0;
    context->field3EE = 0;
    context->field3EF = 0;
    context->turnDelay = 0;
    context->approachTurns = 0;
    context->flags = 0;
    *(struct CrowPackedPosition *)&actor->body.position =
        *(struct CrowPackedPosition *)&context->storedPosition;
    actor->terrainCell = context->homeCell;
    actor->groundHeight2BE =
        SceneActorTerrain_GetHeight(actor->terrainCell, &context->storedPosition);
    actor->field2EA = actor->body.position.y;
    actor->body.position.y = actor->groundHeight2BE;
    if (!actor->drawableKind)
        *(struct CrowPackedPosition
              *)(&((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position) =
            *(struct CrowPackedPosition *)&actor->body.position;
    else
        *(struct CrowPackedPosition *)(&((struct SceneActorRegistryTileDrawable *)actor->drawable)
                                            ->position) =
            *(struct CrowPackedPosition *)&actor->body.position;
    Crow_FillResetPosition(&context->vectors[0], &zero, 0);
    Crow_FillResetPosition(&context->vectors[1], &zero, 0);
    Crow_FillResetPosition(&context->vectors[2], &zero, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    DisplaySequence_Step(actor->drawable);
}

void Crow_UpdateGroundHeight(struct SceneActorRegistryResetView *actor)
{
    struct Crow *state = actor->context;
    u32 kind;
    if (SceneActor_ActionsBlocked(actor))
        return;
    kind = actor->field2B5;
    if (kind == 0 || kind == 0x25 || kind == 0x26) {
        actor->body.position.y = actor->field2EA;
        return;
    }
    {
        u32 mask = 0x100000;
        if (actor->flags2D0 & mask) {
            if (kind != 0x23)
                goto adjust;
            {
                s32 cell = SceneActorTerrain_GetCellIndex(&actor->body.position);
                s32 height = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
                if ((s16)actor->field2EA < height + 0x118) {
                    actor->field2EA += Crow_ScaleFixed12(gUnk_086149C4[state->bobPhase] * 8);
                    if (state->bobPhase <= 0x3F)
                        state->bobPhase++;
                }
            }
        }
    }
    actor->body.position.y = actor->field2EA;
    return;
adjust: {
    s32 diff;
    s32 magnitude;
    u32 ground;
    s32 bias;
    ground = actor->groundHeight2BE;
    bias = -0x118;
    diff = ground - (actor->body.position.y + bias);
    magnitude = diff;
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude > 0x10) {
        if (diff < 0)
            actor->field2EA -= 2;
        else
            actor->field2EA += 2;
    }
}
    actor->body.position.y = actor->field2EA;
    if ((u8)(actor->field2B5 - 1) <= 1) {
        struct DisplaySequenceState *seq;
        struct CrowDrawable *drawable;
        drawable = actor->drawable;
        seq = &drawable->sequence;
        switch (seq->index) {
            case 0:
                *(u16 *)&drawable->sprite.y = *(u16 *)&drawable->sprite.y + 0x18;
                break;
            case 1: {
                u32 base = *(u16 *)&drawable->sprite.y + 0x18;
                *(u16 *)&drawable->sprite.y = base + (seq->elapsed << 2);
                break;
            }
            case 2: {
                u32 base = *(u16 *)&drawable->sprite.y + 0x3E;
                *(u16 *)&drawable->sprite.y = base - seq->elapsed * 12;
                break;
            }
            case 3: {
                u32 base = *(u16 *)&drawable->sprite.y + 2;
                *(u16 *)&drawable->sprite.y = base - (seq->elapsed << 3);
                break;
            }
            case 4: {
                u32 base = *(u16 *)&drawable->sprite.y - 0x16;
                *(u16 *)&drawable->sprite.y = base + seq->elapsed * 7;
                break;
            }
        }
    }
    return;
}

void SceneActorCollision_InitBody(u8 *, struct CrowColliderVector *, struct CrowColliderVector *,
                                  void (*)(void), u8 *);

void SceneActorCollision_InitAttack(u8 *, struct CrowColliderVector *, struct CrowColliderVector *,
                                    void (*)(void), u32, u8 *);

void Crow_InitColliders(u8 *p)
{
    struct CrowColliderVector s1, s2, s3, s4;
    s1.a.lo = 0x2a;
    s1.a.hi = 0x80;
    s1.b.lo = 0x2a;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &s1, &s2, Crow_OnBodyContact, p);
    s3.a.lo = 0x10;
    s3.a.hi = 0x5c;
    s3.b.lo = 0x10;
    *(u32 *)&s4.a = 0;
    *(u32 *)&s4.b &= 0xFFFF0000;
    SceneActorCollision_InitAttack(p, &s3, &s4, Crow_OnAttackContact, 0, p);
}

extern u32 gUnk_08605EA8[];

u32 Crow_GetTimedAction(struct SceneActorRegistryResetView *actor)
{
    s32 v = (s32)actor->counter2AC;
    s32 i = 0;
    if (v > 0x1d) {
        i = 1;
        if (v > 0x31) {
            i = 3;
            if (v <= 0x45)
                i = 2;
        }
    }
    return gUnk_08605EA8[i];
}

void Crow_InitProfile(struct SceneActorRegistryResetView *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(0x13, 0);
}

void Crow_DisableAttackCollision(struct SceneActorRegistryResetView *actor)
{
    struct Crow *q = actor->context;
    u32 m = 0x100000;
    u32 *p = &actor->flags2D0;
    u32 *p2;
    u32 m2;
    *p = *p | m;
    p2 = &q->flags;
    m2 = ~1;
    *p2 = *p2 & m2;
    actor->colliderB.flags &= ~4;
}

void Crow_GetDirectionalFrameAndFlip(u16 heading, s32 *variant, s32 *flags)
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

static inline s32 Crow_GetCollisionHeight(s32 cell, struct ContactPosition *p)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= *(u8 *)&p->z;
            break;
        case 2:
            height -= *(u8 *)&p->x;
            break;
    }
    return height;
}

static inline void Crow_AddMotion(struct ContactPosition *p, struct ContactPosition *a)
{
    p->x = a->x + p->x;
    p->y = a->y + p->y;
    p->z = a->z + p->z;
}

static inline void Crow_ClearFlags(struct Crow *p, u32 m)
{
    p->flags &= ~m;
}

static inline void Crow_SetFlags(struct Crow *p, u32 m)
{
    p->flags |= m;
}

void Crow_StopAtRaisedTerrain(struct SceneActorRegistryResetView *actor)
{
    struct Crow *context = actor->context;
    struct ContactPosition target;
    Crow_ClearFlags(context, 2);
    if (actor->field2B5 == 0x25) {
        struct ContactPosition *p = &target;
        struct ContactPosition *position = &actor->body.position;
        struct ContactPosition *offset = &actor->secondaryMotion;

        s32 cell, height;

        p->x = offset->x + position->x;
        p->y = offset->y + position->y;
        p->z = offset->z + position->z;
        Crow_AddMotion(p, &actor->body.motion);
        cell = SceneActorTerrain_GetCellIndex(&target);
        height = Crow_GetCollisionHeight(cell, &target);
        if (actor->groundHeight2BE + (s16)actor->field2EA < height) {
            actor->secondaryMotion.x = 0;
            actor->secondaryMotion.y = 0;
            actor->secondaryMotion.z = 0;
            context->vectors[2].x = 0;
            context->vectors[2].y = 0;
            context->vectors[2].z = 0;
            actor->body.motion.x = 0;
            actor->body.motion.z = 0;
            actor->body.motion.y = 0;
            Crow_SetFlags(context, 2);
        }
        if (target.y < height) {
            actor->secondaryMotion.y = 0;
            actor->body.motion.y = 0;
            context->vectors[2].y = 0;
        }
    }
}

void Crow_PlayAnimationCue(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *q = &((struct CrowDrawable *)actor->drawable)->sequence;
    if (q->index == 5 && q->elapsed == 0)
        Sound_PlaySong(0xf6);
}

static inline u8 Crow_HasPlayerDetection(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *q = &actor->tracking;
    u32 mask = 2;
    if (q->flags & mask)
        return 1;
    return 0;
}

s32 Crow_TryActivatePlayerProximityResponse(struct SceneActorRegistryResetView *actor)
{
    s32 d;
    struct SceneActorChaseTracking *p;
    if (!Crow_HasPlayerDetection(actor))
        goto zero;
    d = actor->body.position.y - gPlayers[0]->position.y;
    if (d < 0)
        d = -d;
    if (d > 0x200)
        goto zero;
    if (actor->tracking.distance < actor->tracking.limit)
        goto yes;
zero:
    return 0;
yes: {
    u32 mask = 0x80;
    u16 *flags = &actor->flags2D6;
    mask |= *flags;
    *flags = mask;
}
    p = &actor->tracking;
    {
        u32 mask = 0x10;
        mask |= p->flags;
        p->flags = mask;
    }
    return 1;
}

static inline u8 Crow_GetReactionTracking(struct SceneActorRegistryResetView *actor,
                                          struct SceneActorChaseTracking **out)
{
    struct SceneActorChaseTracking *p = &actor->tracking;
    u32 mask = 8;
    u32 value = p->flags & mask;
    *out = p;
    if (value)
        return 1;
    return 0;
}

void Crow_BeginReaction(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x100;
    struct SceneActorChaseTracking *extra;
    u32 zero;
    if (actor->flags2D4 & mask)
        return;
    zero = Crow_GetReactionTracking(actor, &extra);
    if (zero == 0) {
        void (*fn)(void) = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u8 one;
        u8 zero8;
        u32 *flags;
        u8 *taken;
        u32 three;
        taken = &actor->field2B2;
        one = 1;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = code;
        actor->target = fn;
        zero8 = 0;
        actor->field2C8 = zero;
        flags = &actor->flags114;
        {
            u32 clear = -2;
            *flags &= clear;
        }
        actor->word104 = zero;
        three = 3;
        actor->state2A8 = zero8;
        actor->phase2A9 = zero8;
        actor->behavior2AA = three;
        actor->counter2AC = zero;
        actor->field2B1 = one;
        {
            u32 bit = 8;
            bit |= extra->flags;
            extra->flags = bit;
        }
        {
            u32 bit = 0x100000;
            actor->flags2D0 |= bit;
        }
    }
}

static inline s32 Crow_ReadScriptValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}

static inline u32 Crow_GetPositionCell(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

inline static s32 Crow_GetGroundHeight(u16 cell, struct ContactPosition *position)
{
    struct SceneActorChaseTerrainOverride *override =
        (void *)TerrainModifiers_FindCellOverride(cell, 1);
    struct SceneActorChaseTerrainCell *data;
    u32 type;
    s32 height;
    if (override)
        data = &override->properties;
    else
        data = &((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->cells[cell];
    height = data->attributes;
    type = (u32)height >> 4;
    {
        u32 level = 15;
        level &= height;
        height = level << 8;
    }
    {
        u32 slope;
        if (type == 1)
            goto alongZ;
        if (type == 2)
            goto alongX;
        goto finished;
    alongZ:
        slope = (u8)position->z;

        goto subtract;
    alongX:
        slope = (u8)position->x;

    subtract:
        height -= slope;
    }
finished:
    return height;
}

void Crow_ReadSpawnScript(void *input)
{
    struct SceneActorScriptPositionView *actor = input;
    struct Crow *s = input;
    if (Script_SeekToKeyword(0x50)) {
        u32 value;
        u32 distance;
        u32 offset;
        {
            u32 *radius = &(s->orbitRange);
            *radius = Crow_ReadScriptValue();
        }
        s->speed = Crow_ReadScriptValue();
        distance = Crow_ReadScriptValue();
        s->distance = distance;
        if (((u8)distance) <= 0x19)
            s->distance = 0x1A;
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        value = *((const u16 *)((gRandomIndex * 2) + ((u32)gRandomTable)));
        value &= 0x3F;
        if (Script_GetPc())
            offset = Script_GetValue() + value;
        else
            offset = value;
        s->approachTime = offset;
        if (Script_GetPc() && Script_GetValue()) {
            u32 mask = 4;
            u32 *flags = &(s->flags);
            *flags |= mask;
        }
        {
            u32 *radius = &(s->orbitRange);
            *radius = (*radius) * (*radius);
        }
    }
    ((struct Crow *)actor)->body.direction = Script_GetKeywordValue(0x64, 0);
    if (Script_SeekToKeyword(0x70)) {
        u16 *height;
        struct CrowPackedPosition *copy;
        actor->position.x = Crow_ReadScriptValue();
        actor->position.y = Crow_ReadScriptValue();
        {
            u8 *pc = Script_GetPc();
            s32 value;
            height = &actor->position.y;
            if (pc)
                value = Script_GetValue();
            else
                value = 0;
            actor->position.z = value;
        }
        copy = (void *)&s->storedPosition;
        *copy = *((struct CrowPackedPosition *)(&actor->position));
        actor->cellIndex = Crow_GetPositionCell((struct ContactPosition *)&s->storedPosition);
        {
            u32 cell = actor->cellIndex;
            s32 gh;
            gh = Crow_GetGroundHeight(cell, (void *)&s->storedPosition);
            actor->groundHeight = gh;
        }
        {
            u16 y = *height;
            actor->heightOffset = y;
            s->heightOffset = y;
        }
        *height = actor->groundHeight;
        {
            u16 cell = actor->cellIndex;
            s->homeCell = cell;
        }
        {
            u16 gh = actor->groundHeight;
            s->groundHeight = gh;
        }
    }
}

u32 Crow_UpdatePreAction(struct SceneActorRegistryResetView *actor)
{
    Crow_UpdateGroundHeight(actor);
    return 1;
}

u32 Crow_UpdatePostAction(struct SceneActorRegistryResetView *actor)
{
    Crow_StopAtRaisedTerrain(actor);
    return 1;
}

static inline u8 Crow_TakeKnockbackEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void Crow_SetKnockbackState(struct SceneActorRegistryResetView *actor, u8 state,
                                          u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Crow_UpdateKnockback(struct SceneActorRegistryResetView *actor)
{
    struct Crow *state = actor->context;
    u32 mask = 0x42;
    if (Crow_TakeKnockbackEntry(actor)) {
        SceneActorChase_SetControl(actor, Crow_OnSpecialAnimation, 0x26);
        state->vectors[2].y = 30;
    }
    if (((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(actor->terrainCell))
            ->flags &
        mask) {
        if ((s32)actor->counter2AC > 20) {
            Crow_SetKnockbackState(actor, 0, 0, 4);
            return;
        }
    } else {
        u32 mask2 = 0xA02F;
        u16 *speed;
        if (actor->flags2D4 & mask2) {
            u16 *t = &actor->field2C2;
            *t = 0x40;
            speed = t;
        } else {
            u16 *t = &actor->field2C2;
            u32 v = *t;
            speed = t;
            if (v == 0)
                *speed = 0x40;
        }
        if (actor->drawableKind == 0)
            Anim_SetDurationScale(&((struct CrowDrawable *)actor->drawable)->sequence, *speed);
        else
            Anim_SetFrameDurationScale(
                &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *speed);
        actor->field2EA += state->vectors[2].y;
        if (Mod(actor->counter2AC, 4) == 0) {
            s32 v = state->vectors[2].y;
            s32 h;
            if (v >= 0)
                h = v >> 1;
            else
                h = -(-v >> 1);
            state->vectors[2].y = h;
        }
        if (state->vectors[2].y <= 0) {
            Crow_SetKnockbackState(actor, 0, 2, 1);
            return;
        }
    }
    actor->counter2AC++;
}

s32 Div(s32, s32);

void CpuSet(const void *, void *, u32);

static inline u8 Crow_TestFlagsValue(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}

static inline s32 Crow_ShiftMotion(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline void Crow_MakeRecoilVelocity(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Crow_ShiftMotion(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Crow_ShiftMotion(gUnk_086149C4[angle & 255] * speed, 12);
}

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

static inline void Crow_SetAnimationSpeed(struct SceneActorRegistryResetView *actor)
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
        Anim_SetDurationScale(&((struct CrowDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
}

static inline void Crow_FillResponseMotion(void *dst, u32 *tmp, u32 value)
{
    *tmp = value;
    CpuSet(tmp, dst, 0x05000002);
}

static inline void Crow_SetResponseState(struct SceneActorRegistryResetView *actor, u32 state,
                                         u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u8 Crow_TestResponseFlags(struct Crow *body, u32 mask)
{
    return Crow_TestFlagsValue(body->flags, mask);
}

// confidence=low: advances a directed response, then recoils when it reaches the player or terrain.

static inline u8 Crow_ApproachSavedPosition(struct SceneActorRegistryResetView *actor,
                                            struct SceneActorMotionSubject *saved,
                                            struct SceneActorBody *motion, s32 speed)
{
    s16 dx = saved->position.x - ((u16)motion->position.x);
    s16 dz = saved->position.z - ((u16)motion->position.z);
    if ((((dx < 0) ? (-dx) : (dx)) <= 32) && (((dz < 0) ? (-dz) : (dz)) <= 32))
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        *(u16 *)&motion->motion.x +=
            Crow_ShiftMotion(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        *(u16 *)&motion->motion.z += Crow_ShiftMotion(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

void Crow_ApproachPlayerThenRecoil(struct SceneActorRegistryResetView *actor)
{
    struct Crow *body = actor->context;
    u32 zero[1];
    struct ContactPosition delta;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        struct ContactPosition *saved;
        SceneActorChase_SetControl(actor, Crow_UpdateDirectionalPose, 0x25);
        saved = (void *)(&body->curveSubject.position);
        *((struct CrowPackedPosition *)saved) =
            *(struct CrowPackedPosition *)&gPlayers[0]->position;
        Crow_FillResponseMotion(&body->vectors[2], zero, 0);
        Crow_FindPositionDelta(&delta, &actor->body.position, saved);
        actor->body.direction = Math_Atan2(delta.x, delta.z);
        body->vectors[2].x = Crow_ShiftMotion(
            gUnk_086149C4[(actor->body.direction + 64) & 255] * body->distance, 12);
        {
            s32 depth = Crow_ShiftMotion(gUnk_086149C4[actor->body.direction] * body->distance, 12);
            {
                typeof(body->vectors[2].z) *q1 = &(body->vectors[2].z);
                u32 z1 = 0;
                *q1 = depth;
                body->vectors[2].y = z1;
            }
        }
        if (body->vectors[2].x)
            body->vectors[2].x = Div(delta.x, body->vectors[2].x);
        if (body->vectors[2].z)
            body->vectors[2].z = Div(delta.z, body->vectors[2].z);
        if ((body->vectors[2].x) > (body->vectors[2].z)) {
            if (body->vectors[2].x)
                body->vectors[2].y = Div(delta.y - 32, body->vectors[2].x);
        } else if (body->vectors[2].z)
            body->vectors[2].y = Div(delta.y - 32, body->vectors[2].z);
        Sound_PlaySong(247);
        body->bobPhase = 0;
    } else {
        Callback_InvokeActor(actor, actor->contactQuery.opaque);
        Crow_SetAnimationSpeed(actor);
        *(s16 *)&actor->field2EA += body->vectors[2].y;
        if (!Mod(actor->counter2AC, 6))
            body->vectors[2].y = Crow_ShiftMotion(body->vectors[2].y, 1);
        if (Crow_ApproachSavedPosition(actor, &body->curveSubject, &actor->body, body->distance) ||
            Crow_TestResponseFlags(body, 2)) {
            Crow_MakeRecoilVelocity(&actor->secondaryMotion, actor->body.direction,
                                    body->distance * 2);
            Crow_SetResponseState(actor, 0, 2, 2);
        } else {
            actor->counter2AC++;
        }
    }
}

static inline u8 Crow_TakeHeightReturnEntry(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline u32 Crow_ReadHeightReturnFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline void Crow_ResetReturnState(struct SceneActorRegistryResetView *actor)
{
    u32 one = 1;
    u8 *state = &actor->state2A8;
    u32 zero = 0;
    *state = zero;
    actor->phase2A9 = zero;
    actor->behavior2AA = one;
    actor->counter2AC = zero;
    actor->field2B1 = one;
}

void Crow_ReturnToGround(struct SceneActorRegistryResetView *actor)
{
    s32 delta;
    u16 *height;
    if (Crow_TakeHeightReturnEntry(actor)) {
        void *fn = actor->entryTarget228;
        u32 one = 1;
        u8 *taken = &actor->field2B2;
        u32 zero = 0;
        *taken = one;
        actor->field2B0 = zero;
        actor->field2B5 = one;
        actor->target = fn;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = zero;
        actor->body.direction += 0x80;
    }
    Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (Crow_ReadHeightReturnFlags(actor, 0xA02F)) {
        actor->field2C2 = 0x40;
        height = &actor->field2C2;
    } else {
        u16 value = actor->field2C2;
        height = &actor->field2C2;
        if (!value)
            *height = 0x40;
    }
    if (!actor->drawableKind)
        Anim_SetDurationScale(&((struct CrowDrawable *)actor->drawable)->sequence, *height);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *height);
    {
        s32 y;
        s32 offset;
        delta = actor->groundHeight2BE;
        y = actor->body.position.y;
        offset = -280;
        y += offset;
        delta -= y;
    }
    if (delta < 0)
        delta = -delta;
    if ((delta <= 16) && (((s32)actor->counter2AC) > 80)) {
        Crow_ResetReturnState(actor);
    } else {
        ++actor->counter2AC;
    }
}

extern u8 gUnk_08605EB8[];

void Crow_SetBehaviorTable(struct SceneActorRegistryResetView *actor)
{
    actor->behaviorCallbacks = gUnk_08605EB8;
}

void SceneActorControl_DispatchControl(u8 *);

u32 Crow_Update(struct SceneActorRegistryResetView *actor)
{
    u32 i;
    void **t;
    Crow_UpdatePreAction(actor);
    i = actor->state2A8;
    t = actor->callbacks27C;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    Crow_UpdatePostAction(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Crow_InitPlayerTarget(struct SceneActorRegistryResetView *actor)
{
    actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActor_UpdateActiveCallbacks(actor);
    return 0;
}

void Anim_Remove(void *);

void SceneMessages_UnregisterReceiver(void *);

void Shadow_Destroy(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void SceneActorRegistry_UnlinkActor(void *);

static inline u8 Crow_TestDestroyFlags(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

u32 Crow_Destroy(struct SceneActorRegistryResetView *actor)
{
    u8 *p = actor->drawable;
    u8 *q;
    u32 m;
    u32 v;
    if (actor->drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(&((struct SceneActorAnimationTileDrawable *)p)->tile);
    Heap_Free(p);
    m = 0x100;
    if (Crow_TestDestroyFlags(&actor->flags2C0, m))
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

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolveDamage(void);

void SceneActorPlacement_PlayResponseAndResetState(void);

void SceneActorScript_OnCommand(void);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_08605EC4[];

extern SceneActorCommandHandler const gUnk_086108B0[];

static inline void Crow_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, ((struct Crow *)arg0)->movementProbe, 0x40, 0x40);
}

static inline void Crow_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                    u16 alternate)
{
    s32 r1v;
    u32 currentPalette;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    currentPalette = MonsterInit_GetPaletteOffset(actor);
    currentPalette += 0x1CE;
    MonsterInit_SetAlternatePaletteOffset(actor, (u16)currentPalette);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(((struct CrowDrawable *)actor->drawable)->animation, r1v);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
            gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
            SceneActorScript_GetManagerField62());
}

static inline void Crow_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Crow_InitPlayerTarget;
    actor->destroyCallback = Crow_Destroy;
}

static inline void Crow_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_08605EC4);
    actor->callback248 = Crow_ResetGroundState;
    actor->field23C = Crow_BeginReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Crow_TryActivatePlayerProximityResponse);
}

s32 Crow_Init(void *object)
{
    struct Crow *crow = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    crow->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x13);
    Crow_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x5A6E, 0x7B03);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            ((struct ActorSprite *)heap)->priority = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5A6E, &tmp);
        actor->animationIndex = 0xFF;
    }
    Crow_InitializeBody(actor, &zero);
    Crow_ReadSpawnScript(crow);
    Crow_ReadPalette(actor, 0x1CB, 0x192);

    Crow_InitColliders(actor);
    MonsterInit_InstallOpaqueContactCallbacks(
        actor, Crow_Update, Crow_GetTimedAction, Crow_PlayEntry2, Crow_OnIdleAnimation,
        Crow_UpdatePeriodicAnimation, 0, Crow_RecoverFromDisplacement, Crow_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, 0, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    Crow_InstallRoutes(actor);
    MonsterInit_SetMotionFlags(actor, 0x60);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
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
        MonsterInit_MaskColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                  actor->drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Crow_InitActionTable(actor);
    Crow_SetBehaviorTable(actor);
    actor->field2E8 = 2;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Crow_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

void Crow_Create(void)
{
    void *m = Heap_Alloc(0x43c);
    if (m != 0) {
        Memory_Clear(m, 0x43c);
        if (Crow_Init(m) < 0) {
            Crow_Destroy(m);
            Heap_Free(m);
        }
    }
}
