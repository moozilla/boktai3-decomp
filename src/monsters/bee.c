#include "monsters/bee.h"

#include "global.h"
#include "engine/math/math_shift.h"

#include "scene/actor/scene_actor_control_phase.h"

#include "npc/manager/scene_actor_animation.h"

#include "player/player.h"

#include "scene/actor/scene_actor_behavior.h"

#include "script/script_utility.h"

#include "scene/motion/scene_actor_terrain.h"

#include "engine/engine.h"

#include "engine/ram.h"

#include "scene/actor/scene_actor_control_motion.h"

#include "engine/video/display_sequence_inline.h"

#include "scene/collision/contact_actor.h"

#include "engine/math/movement_curve.h"

#include "scene/actor/scene_actor_reset.h"

#include "scene/actor/scene_actor_reaction.h"

#include "scene/actor/scene_actor_state.h"

#include "scene/collision/scene_actor_collision.h"

#include "scene/collision/scene_body_api.h"

#include "engine/video/render.h"

#include "engine/engine_globals.h"

#include "monsters/scene_actor_init_group.h"

#include "monsters/scene_actor_init_graphics.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_displacement.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/motion/scene_actor_chase.h"
#include "bosses/vanargand/vanargand_parts.h"

void Bee_OnAttackCollision(void)
{
}

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct RoomActor *);

void Bee_ApplyContactReaction(struct SceneActorBehaviorContactView *source,
                              struct SceneActorBehaviorContactView *destination,
                              struct ChaseActor *actor)
{
    u32 index;
    s32 power;
    u32 sample;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (SceneActor_MaskedHalfword(0x800, &source->flags))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    sample = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    power = destination->power - (sample & 15);
    power = power < 20 ? 20 : power;
    destination->power = power;
    {
        struct SceneActorControlState *control = SceneActorControl_GetState(actor);
        u32 *flags = &control->flags3D4;
        u32 mask = ~4;
        *flags &= mask;
    }
}

static inline void Bee_SelectAnimation(struct RoomActor *actor, u32 selection, u32 variant,
                                       u32 flags)
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

void Bee_UpdateTurnAnimation(struct RoomActor *actor)
{
    struct Bee *body = (void *)actor->context;
    u32 variant, flags;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        Bee_GetDirectionalFrameAndFlip(actor->body.direction, &variant, &flags);
        {
            struct DisplaySequenceState *sequence =
                &((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
            sequence->frameCount = 0;
        }
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        {
            u32 direction = actor->body.direction;
            body->savedFacing = direction;
        }
    } else if (body->savedFacing != actor->body.direction) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        struct DisplaySequenceState *sequence =
            &((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
        u32 frame = sequence->index, time = sequence->elapsed;
        Bee_GetDirectionalFrameAndFlip(actor->body.direction, &variant, &flags);
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        sequence->index = frame;
        sequence->elapsed = time;
        {
            u32 direction = actor->body.direction;
            body->savedFacing = direction;
        }
    } else
        actor->restart = 0;
}

void Bee_ResetIdleAnimation(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
}

void Bee_ResetReactionAnimation(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0xe);
}

static inline u8 Bee_TakeReadyFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void Bee_UpdateRecoveryAnimation(struct ChaseActor *actor)
{
    if (Bee_TakeReadyFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if (actor->base.restart)
        actor->base.stepDone = 1;
}

// confidence=low: preserves sequence timing when the Bee changes facing without entering a state.

void Bee_UpdateFacingAnimation(struct RoomActor *actor)
{
    struct Bee *body = (void *)actor->context;
    u32 variant, flags;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        Bee_GetDirectionalFrameAndFlip(actor->tracking.heading, &variant, &flags);
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        SceneActorAnimation_SelectDirectionalEntry(actor, 1);
        {
            u32 direction = actor->tracking.heading;
            body->savedFacing = direction;
        }
    } else if (body->savedFacing != actor->tracking.heading) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        struct DisplaySequenceState *sequence =
            &((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
        u32 frame = sequence->index, time = sequence->elapsed;
        Bee_GetDirectionalFrameAndFlip(actor->tracking.heading, &variant, &flags);
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        sequence->index = frame;
        sequence->elapsed = time;
        {
            u32 direction = actor->tracking.heading;
            body->savedFacing = direction;
        }
    } else
        actor->restart = 0;
}

void SceneActorControl_ResetCollisionPhase(struct ChaseActor *);

void SceneActorControl_StartPositionCurve(struct ChaseActor *, struct SceneVec *);

static inline s32 Bee_ClampMotion(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}

static inline u8 Bee_IsMotionAtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

// confidence=low: eases saved motion before returning to idle.

static inline void Bee_SetRouteFlags(struct Bee *body, u32 mask)
{
    body->flags |= mask;
}

void Bee_UpdateMovementStop(struct ChaseActor *actor)
{
    struct Bee *body = actor->base.context;
    s32 x, z;
    s16 *priorX, *priorZ;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        u16 *flags;
        {
            u32 value = 0x20;
            flags = &actor->base.motionFlags;
            value |= *flags;
            *flags = value;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
    }
    MonsterInit_SetStatusFlags(actor, 0x100000);
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
    x = Bee_ClampMotion(x);
    z = Bee_ClampMotion(z);
    actor->base.body.motion.x += x;
    actor->base.body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (Bee_IsMotionAtRest(x, z)) {
        actor->base.stepDone = 1;
        {

            struct BeeSignedFlag *f = (void *)&actor->base.motionFlags;
            s32 m = ~0x20;
            m &= (u16)f->bits;
            f->bits = m;
        }
        {
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x80);
        }
        Bee_DisableMovement(actor);
        SceneActorTerrain_EnterState(actor, 0, 2, 0);
        Bee_SetRouteFlags(body, 8);
        body->routeCooldown = 60;
    }
}

/* SceneActorControl_CompleteScriptedResponse_0809F368: SceneActorControl_CompleteScriptedResponse with other constants (ported by relatives.py, fixed by hand; provisional name) */

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(struct RoomActor *);

void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void Bee_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            u16 *flags = &actor->base.colliderA.flags;
            {
                u32 v = *flags;
                u32 m = 4;
                m |= v;
                *flags = m;
            }
        }
    }
    if (!Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30))
        return;
    actor->base.stepDone = 1;
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
    if (!Flags_MaskedWord(0x400, &actor->base.behaviorFlags))
        SceneActorBehavior_CallTerrainScript((void *)actor);
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    SceneActorTerrain_EnterState(actor, 1, 0, 2);
    ActorShadow_Disable(actor->base.shadow);
}

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

s32 Div(s32, s32);

void CpuSet(const void *, void *, u32);

static inline u32 Bee_ReturnRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}

static inline void Bee_EnterState(struct ChaseActor *a, u32 phase, u32 behavior)
{
    a->base.state2A8 = 0;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

// confidence=low: initializes a small random movement and returns to the player when it stops.

void Bee_ReturnToPlayerWhenStopped(struct ChaseActor *actor)
{
    s32 length;
    struct ContactPosition *motion;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        struct Bee *body = actor->base.context;
        u32 angle;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&body->motion);
        angle = (u8)Bee_ReturnRandom();
        {
            const s16 *table = gUnk_086149C4;
            s32 vx = Math_ShiftTowardZero(table[angle], 6);
            actor->base.secondaryMotion.x = vx;
        }
        {
            const s16 *table = gUnk_086149C4;
            s32 vz = Math_ShiftTowardZero(table[(angle + 64) & 255], 6);
            actor->base.secondaryMotion.z = vz;
        }
    }
    motion = &actor->base.secondaryMotion;
    length = motion->x * motion->x + motion->z * motion->z;
    if (!length) {
        Bee_DisableMovement(actor);
        ZeroFill8(motion);
        Bee_BeginMoveToPosition(actor,
                                &((struct SceneActorChasePlayer *)gPlayers[0])->target.position);
        Bee_EnterState(actor, 2, 0);
    }
    actor->base.counter2AC++;
}

extern void *gUnk_08605D64[];

void Bee_InitActionPreset(struct ChaseActor *actor)
{
    actor->base.callbacks284 = gUnk_08605D64;
}

u8 *SceneBody_GetById(int);

static inline u32 Bee_AnchorRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + 0x0203B400);
}

static inline u32 Bee_AnchorActorFlags(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->base.motionFlags & mask : actor->base.statusFlags & mask;
}

static inline u8 Bee_AnchorUnavailable(struct ChaseActor *actor)
{
    if (actor && (Bee_AnchorActorFlags(actor, 0x40, 0x2D4) ||
                  Bee_AnchorActorFlags(actor, 0x20000, 0x2D0) || actor->base.health <= 0))
        return 1;
    return 0;
}

static inline void Bee_AnchorPolarOffset(struct ContactPosition *out, u16 angle, s32 radius)
{
    out->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 0x40) & 255] * radius, 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 255] * radius, 12);
}


static inline void Bee_ClearAnchorMotion(u32 *zero, void *out)
{
    *zero = 0;
    CpuSet(zero, out, 0x05000002);
}

inline static void Bee_AddTargetOffset(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x = a->x + b->x;
    a->y = a->y + b->y;
    a->z = a->z + b->z;
}

/* Tentative context view for body-directed movement. */

static inline s32 Bee_GetBearing(const struct ContactPosition *delta)
{
    return Math_Atan2(delta->x, delta->z);
}

void Bee_StartMoveToBody(struct ChaseActor *actor, u16 *args)
{
    struct Bee *body;
    struct {
        u32 fill;
        struct ContactPosition delta;
    } work;
    struct ContactPosition offset;
    u32 angle;

    u16 parameter;
    if (Bee_AnchorUnavailable(actor))
        return;
    if (!Bee_AnchorActorFlags(actor, 0x4000, 0x2D4))
        return;
    body = actor->base.context;
    {
        u16 value = args[1];
        body->bodyId = value;
    }
    body->anchor = (void *)SceneBody_GetById(body->bodyId);
    parameter = args[2];
    body->polarSpeed = parameter;
    body->index = 0;
    Chase_Delta(&work.delta, &actor->base.body.position, &body->anchor->position);
    actor->base.tracking.heading = Bee_GetBearing(&work.delta);
    angle = ((Bee_AnchorRandom() & 0x3F) + actor->base.tracking.heading) & 0xFF;
    Bee_AnchorPolarOffset(&offset, angle, 0x200);
    body->remaining = 0x500;
    Bee_AddTargetOffset(&offset, &actor->base.body.position);
    Bee_BuildMovementCurve(actor, &offset);
    Bee_SubVector(actor, &actor->base.body.position, &offset);
    Bee_ClearAnchorMotion(&work.fill, &body->motion);
    MonsterInit_SetState(actor, 2, 0, 10);
    SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
}

u8 *TerrainModifiers_FindCellOverride(int, int);

static inline u8 *Bee_GetTerrainProperties(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}

void Bee_ResetRoute(struct ChaseActor *actor)
{
    u8 *bytes;
    struct Bee *context = actor->base.context;
    struct ContactPosition *source;
    s32 height, slope;
    u8 *c;
    u32 cellIndex;
    context->routeReset = 0;
    context->curveTime = 0;
    context->flightPhase = 0;
    context->curveActive = 1;
    context->curve = 0;
    context->curveDirection = 0;
    context->curveMode = 0;
    context->flags = 0;
    source = &context->initialPosition;
    actor->base.body.position = *source;
    {
        s32 x = source->x >> 8, z = source->z >> 8, index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        actor->base.terrainCell = index;
        cellIndex = actor->base.terrainCell;
    }
    {
        bytes = (void *)&context->initialPosition;
        c = Bee_GetTerrainProperties(cellIndex);
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= bytes[4];
                break;
            case 2:
                height -= bytes[0];
                break;
        }
    }
    actor->base.groundHeight = height;
    actor->base.heightOffset = actor->base.body.position.y - height;
    actor->base.body.position.y = actor->base.groundHeight;
    if (!actor->base.drawableKind) {
        *(struct BeePackedPosition *)&(
             (struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)
             ->sprite.position = *(struct BeePackedPosition *)&actor->base.body.position;
    } else {
        *(struct BeePackedPosition *)&(
             (struct SceneActorRegistryTileDrawable *)actor->base.drawable)
             ->position = *(struct BeePackedPosition *)&actor->base.body.position;
    }
    ZeroFill8(&context->start);
    ZeroFill8(&context->end);
    ZeroFill8(&context->motion);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    DisplaySequence_Step(actor->base.drawable);
}

static inline u32 Bee_FlightActorFlags(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->base.motionFlags & mask : actor->base.statusFlags & mask;
}

static inline u8 Bee_FlightUnavailable(struct ChaseActor *actor)
{
    if (actor && (Bee_FlightActorFlags(actor, 0x40, 0x2D4) ||
                  Bee_FlightActorFlags(actor, 0x20000, 0x2D0) || actor->base.health <= 0))
        return 1;
    return 0;
}

static inline u8 Bee_HasRouteFlag(const struct Bee *state, u32 bits)
{
    if (state->flags & bits)
        return TRUE;
    return FALSE;
}


void Bee_UpdateFlightHeight(struct ChaseActor *actor)
{
    s32 delta, absolute;
    u16 *g;
    struct Bee *state;
    {
        struct Bee *raw = (void *)SceneActorControl_GetState(actor);
        state = raw;
    }

    if (Bee_FlightUnavailable(actor))
        return;
    g = &actor->base.groundHeight;
    {
        u32 diff = *g - state->previousGround;
        actor->base.heightOffset -= diff;
        state->previousGround = *g;
    }
    if (!Bee_FlightActorFlags(actor, 0x100000, 0x2D0)) {
        actor->base.body.position.y = *g + actor->base.heightOffset;
        return;
    }
    if ((s16)actor->base.heightOffset <= 259) {
        {
            if (Bee_HasRouteFlag(state, 1)) {
                actor->base.body.position.y = actor->base.groundHeight;
                return;
            }
        }
        actor->base.heightOffset += Math_ShiftTowardZero(gUnk_086149C4[state->flightPhase], 9);
        if (state->flightPhase <= 63)
            state->flightPhase++;
        actor->base.body.position.y = actor->base.groundHeight + actor->base.heightOffset;
    }
    {
        s32 currentGround = actor->base.groundHeight;
        s32 raisedY = actor->base.body.position.y - 260;
        delta = currentGround - raisedY;
    }
    absolute = delta >= 0 ? delta : -delta;
    if (absolute > 16) {
        if (delta < 0)
            actor->base.heightOffset -= 8;
        else
            actor->base.heightOffset += 8;
    }
    actor->base.body.position.y = actor->base.groundHeight + actor->base.heightOffset;
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
        drawable->position.y -= Math_ShiftTowardZero(gUnk_086149C4[state->flightPhase], 5);
    }
    if (state->flightPhase > 64)
        state->flightPhase = 0;
    else
        state->flightPhase += 8;
}

void SceneActorCollision_InitBody(u8 *, struct BeeCollisionVector *, struct BeeCollisionVector *,
                                  void (*)(void), u8 *);

void SceneActorCollision_InitAttack(u8 *, struct BeeCollisionVector *, struct BeeCollisionVector *,
                                    void (*)(void), u32, u8 *);

void Bee_InitColliders(u8 *p)
{
    struct BeeCollisionVector s1, s2, s3, s4;
    s1.a.lo = 0x10;
    s1.a.hi = 0x40;
    s1.b.lo = 0x10;
    *(u32 *)&s2.a = 0;
    *(u32 *)&s2.b &= 0xFFFF0000;
    SceneActorCollision_InitBody(p, &s1, &s2, Bee_ApplyContactReaction, p);
    s3.a.lo = 0x20;
    s3.a.hi = 0x34;
    s3.b.lo = 0x20;
    *(u32 *)&s4.a = 0;
    *(u32 *)&s4.b &= 0xFFFF0000;
    SceneActorCollision_InitAttack(p, &s3, &s4, Bee_OnAttackCollision, 0, p);
}

extern const u32 gUnk_08605D68[8];

static inline s32 Bee_QuantizeCurveTime(s32 value)
{
    if (value >= 0)
        return value >> 7;
    return -((-value) >> 7);
}

static inline u32 Bee_ReadMovementCode(const u32 *t, u32 i)
{
    return t[i];
}

u32 Bee_GetMovementAnimationCode(struct ChaseActor *actor)
{
    struct Bee *state = actor->base.context;
    s32 sample;
    s32 index;
    sample = Bee_QuantizeCurveTime(state->curveTime);
    if (sample < 0)
        sample = -sample;
    index = sample;
    if (index > 7)
        index = 7;
    {
        if (Bee_HasRouteFlag(state, 4))
            return Bee_ReadMovementCode(gUnk_08605D68, index) + 6;
        return Bee_ReadMovementCode(gUnk_08605D68, index);
    }
}

void Bee_SetMovementEndpoints(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Bee *context = actor->base.context;
    context->start = actor->base.body.position;
    context->end = *target;
    context->curveTime = 0;
}

u8 Bee_AdvanceMovementCurve(struct ChaseActor *actor)
{
    struct Bee *ctx = actor->base.context;
    struct ContactPosition start, end;
    start = ctx->start;
    end = ctx->end;
    start.y = 0;
    end.y = 0;
    MovementCurve_InterpolatePosition(&ctx->curveSubject.position, &start, &end, 1, ctx->curveTime,
                                      0, ctx->curve);
    if (ctx->curveTime > 1023) {
        ctx->curveTime = 0;
        return 1;
    }
    ctx->curveTime += (s16)MovementCurve_RandomByte() + 256;
    if (ctx->curveTime > 1024)
        ctx->curveTime = 1024;
    return 0;
}

void Bee_InitProfile(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(0x14, 0);
}

void Bee_DisableMovement(struct ChaseActor *actor)
{
    struct Bee *context = actor->base.context;
    MonsterInit_SetStatusFlags(actor, 0x100000);
    SceneActorChase_ClearWordFlags(&context->flags, 3);
    actor->base.colliderB.flags &= ~4;
}

void Bee_BeginMoveToPosition(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Bee *context = actor->base.context;
    struct ContactPosition delta;
    Bee_SetMovementEndpoints(actor, target);
    Bee_AdvanceMovementCurve(actor);
    {
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        Chase_Delta(&delta, &actor->base.body.position, target);
        actor->base.nextDirection = Bee_GetBearing(&delta);
        ZeroFill8(&context->motion);
    }
}

void Bee_GetDirectionalFrameAndFlip(u16 a, s32 *b, s32 *c)
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

void Bee_TickBehaviorCooldown(struct ChaseActor *actor)
{
    struct Bee *context = actor->base.context;
    u8 v = context->behaviorCooldown;
    if (v != 0)
        context->behaviorCooldown = v - 1;
}

static inline void Bee_InterpolatePoint(struct ContactPosition *out, struct ContactPosition *start,
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

void Bee_BuildMovementCurve(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Bee *route = actor->base.context;
    struct ContactPosition start;
    struct ContactPosition end;
    s32 step, tx, tz, i;

    start = actor->base.body.position;
    end = *target;
    start.y = 0;
    end.y = 0;
    step = Div(0x400, 8);
    tx = route->motion.x << 4;
    tz = route->motion.z << 4;
    for (i = 0; i < 8; i++) {
        s32 next = i + 1;
        /* Keeping the indexed base before the member address preserves agbcc
         * addition ordering; direct array indexing changes register allocation. */
        struct ContactPosition *out = (void *)(i * sizeof(route->points[0]) + (u32)route);
        out = ((struct Bee *)out)->points;
        Bee_InterpolatePoint(out, &start, &end, tx, tz, step * next);
    }
    route->points[8] = end;
}

void *Bee_SubVector(void *unused, struct BeeDisplacement *a, struct BeeDisplacement *b)
{
    struct BeeDisplacement d;
    struct BeeDisplacement *p = &d;
    d.x = b->x - a->x;
    p->y = b->y - a->y;
    p->z = b->z - a->z;
    return unused;
}

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

static inline u32 Bee_AttackActorFlags(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->base.motionFlags & mask : actor->base.statusFlags & mask;
}

static inline u8 Bee_AttackUnavailable(struct ChaseActor *actor)
{
    if (actor && (Bee_AttackActorFlags(actor, 0x40, 0x2D4) ||
                  Bee_AttackActorFlags(actor, 0x20000, 0x2D0) || actor->base.health <= 0))
        return 1;
    return 0;
}

void SceneActorCollision_FireSecondaryAttack(struct ChaseActor *actor)
{
    if (Bee_AttackUnavailable(actor))
        return;
    {
        if (SceneActorChase_QueryMotionFlagComponent(actor, 0x100)) {
            struct ContactCollider *collider = &actor->base.colliderB;
            u16 field3C = actor->base.attacks.first[1];
            u16 field40 = actor->base.attacks.second[1];
            u32 reactionFlags = actor->base.attacks.flags[1];
            u16 field44 = actor->base.attacks.parameter[1];
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
            return;
        }
    }
    {
        struct ContactCollider *collider = &actor->base.colliderB;
        u16 field3C = actor->base.attacks.first[0];
        u16 field40 = actor->base.attacks.second[0];
        u32 reactionFlags = actor->base.attacks.flags[0];
        u16 field44 = actor->base.attacks.parameter[0];
        Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
    }
}

u32 Bee_OnDetectionCheck(struct ChaseActor *actor)
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

void Bee_SetTrackingReactionFlag(struct ChaseActor *actor)
{
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    MonsterInit_SetTargetingFlags(tracking, 8);
}

u32 Bee_GetMovementVariant(struct ChaseActor *actor)
{
    u32 r;
    switch (actor->base.paletteOffset) {
        case 0:
            r = 7;
            break;
        case 1:
            r = 4;
            break;
        case 2:
            r = 6;
            break;
        default:
            r = 4;
            break;
    }
    return r;
}

void Bee_TickActionCooldown(struct Bee *context)
{
    u8 *q = &context->actionCooldown;
    u8 v = *q;
    if (v != 0)
        *q = v - 1;
}

u32 Bee_UpdateGroundHeight(struct ChaseActor *actor)
{
    Bee_UpdateFlightHeight(actor);
    return 1;
}

u32 Bee_UpdateTimersAndContacts(u8 *p)
{
    Bee_TickBehaviorCooldown(p);
    SceneActorCollision_FireSecondaryAttack(p);
    Bee_TickActionCooldown(p);
    return 1;
}

static inline void Bee_ClearRouteFlags(struct Bee *body, u32 bits)
{
    u32 mask = ~bits;
    body->flags &= mask;
}

static inline u32 Bee_OffsetHeading(u32 value, const u8 *heading)
{
    return (value & 63) + *heading;
}

static inline u16 Bee_GetWaypointX(struct Bee *body, s32 index)
{
    return body->points[index].x;
}

static inline u16 Bee_GetWaypointZ(struct Bee *body, s32 index)
{
    return body->points[index].z;
}

static inline s32 Bee_Max(s32 a, s32 b)
{
    if (a < b)
        return b;
    return a;
}

static inline u16 Bee_NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}

static inline void Bee_MakePolarVector(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 255] * speed, 12);
}

static inline void Bee_AddPosition(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = out->x + from->x;
    out->y = out->y + from->y;
    out->z = out->z + from->z;
}

static inline u8 Bee_GetHeading(const struct ChaseActor *actor)
{
    return actor->base.tracking.heading;
}

// confidence=low: follows a randomized route and returns to the Bee behavior selector.

void Bee_FollowWanderRoute(struct ChaseActor *actor)
{
    struct ContactPosition dest;
    struct ContactPosition delta;
    struct Bee *body = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        body->index = 0;
        if (Bee_HasRouteFlag(body, 8)) {
            Bee_MakePolarVector(
                &dest, (actor->base.tracking.heading + 128 + (Bee_NextRandom() & 63)) & 255, 320);
            Bee_ClearRouteFlags(body, 8);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                s32 duration = 6 << variant;
                body->remaining = duration;
            }
        } else {
            Bee_MakePolarVector(
                &dest, Bee_OffsetHeading(Bee_NextRandom(), &actor->base.tracking.heading) & 255,
                512);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                s32 duration = 40 << variant;
                body->remaining = duration;
            }
        }
        Bee_AddPosition(&dest, &actor->base.body.position);
        Bee_BuildMovementCurve(actor, &dest);
        Bee_SubVector(actor, &actor->base.body.position, &dest);
        ZeroFill8(&body->motion);
    }
    Chase_Delta(&delta, &actor->base.body.position, &body->points[body->index]);
    {
        u8 facing = Bee_GetHeading(actor);
        actor->base.body.direction = facing;
    }
    if ((body->remaining > 0) && (((delta.x * delta.x) + (delta.z * delta.z)) > 1024)) {
        s32 steps = Bee_Max(Math_ShiftTowardZero(body->remaining, 5), 1);
        actor->base.body.motion.x +=
            Div(body->points[body->index].x - actor->base.body.position.x, steps);
        actor->base.body.motion.z +=
            Div(body->points[body->index].z - actor->base.body.position.z, steps);
        body->motion = actor->base.body.motion;
        body->remaining -= actor->base.counter2AC;
    } else if ((++body->index) > 8) {
        actor->base.body.position.x = body->points[8].x;
        actor->base.body.position.z = body->points[8].z;
        Bee_MakePolarVector(&dest, ((Bee_NextRandom() & 63) + actor->base.tracking.heading) & 255,
                            512);
        Bee_AddPosition(&dest, &actor->base.body.position);
        Bee_BuildMovementCurve(actor, &dest);
        Bee_SubVector(actor, &actor->base.body.position, &dest);
        {
            s32 variant = Bee_GetMovementVariant(actor);
            s32 duration = (body->duration) << variant;
            body->remaining = duration;
        }
        body->index = 0;
        actor->base.counter2AC = 0;
        return;
    } else {
        actor->base.body.position.x = Bee_GetWaypointX(body, body->index - 1);
        actor->base.body.position.z = Bee_GetWaypointZ(body, body->index - 1);
    }
    if (((s32)actor->base.counter2AC) <= 19)
        actor->base.counter2AC++;
    if (body->routeCooldown)
        body->routeCooldown--;
    else if (actor->base.tracking.distance <= 0xFFFE) {
        if (body->behaviorCooldown)
            Bee_EnterState(actor, 2, 1);
        else
            Bee_EnterState(actor, 2, 2);
    }
}

void SceneActorControl_TickPendingTarget(struct ChaseActor *actor)
{
    struct Bee *context = actor->base.context;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        actor->base.body.direction = actor->base.tracking.heading;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
    }
    {
        u32 *counter = &actor->base.counter2AC;
        if ((s32)*counter > 30) {
            SceneActorTerrain_EnterState(actor, 0, 2, 0);
            Bee_SetRouteFlags(context, 8);
            context->routeCooldown = 60;
        } else {
            *counter = *counter + 1;
        }
    }
}

static inline void Bee_ResetBehavior(struct ChaseActor *actor, u32 phase, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Bee_UpdateReactionPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        actor->base.body.direction = actor->base.tracking.heading;
        SceneActorChase_SetControl(actor, Bee_ResetReactionAnimation, 0x25);
    }
    if ((s32)actor->base.counter2AC > 0x28 && actor->base.restart)
        Bee_ResetBehavior(actor, 2, 3);
    actor->base.counter2AC++;
}


// confidence=low: approaches a saved player position during the recovery animation.

/* Like Crow_ApproachSavedPosition: push the body toward the saved point; 1 once within 32. */
static inline u8 Bee_ApproachSavedPosition(struct SceneActorMotionSubject *saved,
                                           struct SceneActorBody *motion, s32 speed)
{
    s16 dx = saved->position.x - motion->position.x;
    s16 dz = saved->position.z - motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x +=
            Math_ShiftTowardZero(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        motion->motion.z += Math_ShiftTowardZero(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

void Bee_ApproachSavedPlayerPosition(struct ChaseActor *actor)
{
    struct Bee *body = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        actor->base.body.direction = actor->base.tracking.heading;
        SceneActorChase_SetControl(actor, Bee_UpdateRecoveryAnimation, 0x26);
        *(struct BeePackedPosition *)&body->curveSubject.position =
            *(struct BeePackedPosition *)&((struct SceneActorChasePlayer *)gPlayers[0])
                 ->target.position;
        {
            u32 value = body->behaviorDelay;
            body->behaviorCooldown = value;
        }
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
        Sound_PlaySong(0x3C8);
    }
    Bee_ApproachSavedPosition(&body->curveSubject, &actor->base.body, 24);
    if (actor->base.stepDone)
        Bee_EnterState(actor, 2, 4);
    else
        actor->base.counter2AC++;
}

static inline u32 Bee_PauseRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}

static inline void Bee_PausePolarMotion(struct ContactPosition *m, u32 angle, s32 radius)
{
    m->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius, 12);
    m->y = 0;
    m->z = Math_ShiftTowardZero(gUnk_086149C4[angle] * radius, 12);
}

void Bee_UpdateWanderPause(struct ChaseActor *actor)
{
    s32 cnt;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 angle;
        u32 r;
        u8 *fp;

        SceneActorChase_SetControl(actor, Bee_UpdateFacingAnimation, 0x27);
        r = Bee_PauseRandom();
        fp = &actor->base.tracking.heading;
        angle = (r & 0x1F) + 0x71;
        angle = (angle + *fp) & 0xFF;
        Bee_PausePolarMotion(&actor->base.secondaryMotion, angle, 0x20);
    }
    cnt = actor->base.counter2AC;
    if (cnt > 0x1E) {
        SceneActorChase_ClearMotionFlagComponent(actor, 0x100);
        SceneActorTerrain_EnterState(actor, 0, 2, 0);
    } else {
        actor->base.counter2AC = cnt + 1;
    }
}

static inline void Bee_MakeAnchorVector(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Math_ShiftTowardZero(gUnk_086149C4[angle & 255] * speed, 12);
}

static inline u16 Bee_NextAnchorRandom(void)
{
    u32 offset;
    const u16 *table;

    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    return *(const u16 *)(offset + (u32)table);
}

static inline u32 Bee_OffsetAnchorHeading(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}

static inline u16 Bee_GetPreviousX(struct Bee *route, s32 index)
{
    return route->points[index].x;
}

static inline u16 Bee_GetPreviousZ(struct Bee *route, s32 index)
{
    return route->points[index].z;
}

// confidence=low: steps along nine route points toward an anchor, then retargets after the route ends.

void Bee_FollowAnchorRoute(struct ChaseActor *actor)
{
    struct Bee *route = actor->base.context;
    struct ContactPosition delta, target, step;
    s32 n;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        route->index = 0;
        Chase_Delta(&delta, &actor->base.body.position, &route->anchor->position);
        actor->base.tracking.heading = Math_Atan2(delta.x, delta.z);
        Bee_MakeAnchorVector(
            &target,
            Bee_OffsetAnchorHeading(Bee_NextAnchorRandom(), &actor->base.tracking.heading) & 255,
            route->polarSpeed);
        route->remaining = 0x500;
        Bee_AddPosition(&target, &actor->base.body.position);
        Bee_BuildMovementCurve(actor, &target);
        Bee_SubVector(actor, &actor->base.body.position, &target);
        ZeroFill8(&route->motion);
    }
    Chase_Delta(&step, &actor->base.body.position, &route->points[route->index]);
    Chase_Delta(&delta, &actor->base.body.position, &route->anchor->position);
    {
        u32 heading = Math_Atan2(delta.x, delta.z);
        actor->base.tracking.heading = heading;
        actor->base.body.direction = heading;
    }
    if ((s16)route->remaining > 0 && step.x * step.x + step.z * step.z > 1024) {
        n = Bee_Max(Math_ShiftTowardZero((s16)route->remaining, 5), 1);
        actor->base.body.motion.x +=
            Div(route->points[route->index].x - actor->base.body.position.x, n);
        actor->base.body.motion.z +=
            Div(route->points[route->index].z - actor->base.body.position.z, n);
        route->motion = actor->base.body.motion;
        route->remaining -= actor->base.counter2AC;
    } else {
        route->index++;
        if (route->index > 8) {
            actor->base.body.position.x = route->points[8].x;
            actor->base.body.position.z = route->points[8].z;
            Chase_Delta(&delta, &actor->base.body.position, &route->anchor->position);
            actor->base.tracking.heading = Math_Atan2(delta.x, delta.z);
            Bee_MakeAnchorVector(
                &target,
                Bee_OffsetAnchorHeading(Bee_NextAnchorRandom(), &actor->base.tracking.heading) &
                    255,
                route->polarSpeed);
            Bee_AddPosition(&target, &actor->base.body.position);
            Bee_BuildMovementCurve(actor, &target);
            Bee_SubVector(actor, &actor->base.body.position, &target);
            {
                u32 duration = route->duration << 5;
                route->remaining = duration;
            }
            route->index = 0;
            actor->base.counter2AC = 0;
            return;
        }
        actor->base.body.position.x = Bee_GetPreviousX(route, route->index - 1);
        actor->base.body.position.z = Bee_GetPreviousZ(route, route->index - 1);
    }
    if ((s32)actor->base.counter2AC <= 19)
        actor->base.counter2AC++;
}

extern void *gUnk_08605D88[];

void Bee_InitBehaviorHandlers(struct ChaseActor *actor)
{
    actor->base.behaviorCallbacks = gUnk_08605D88;
}

extern void *gUnk_08605D9C[];

void Bee_InitReactionHandlers(struct ChaseActor *actor)
{
    actor->base.callbacks29C = gUnk_08605D9C;
}

void SceneActorControl_DispatchControl(u8 *);

u32 Bee_OnUpdate(struct ChaseActor *actor)
{
    u32 i;
    void **t;
    Bee_UpdateGroundHeight(actor);
    i = actor->base.state2A8;
    t = actor->base.stateCallbacks;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    Bee_UpdateTimersAndContacts(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Bee_OnPlayerInteraction(struct ChaseActor *actor)
{
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActor_UpdateActiveCallbacks((void *)actor);
    return 0;
}

void Sprite_Remove(void *);

void Anim_Remove(void *);

void SceneMessages_UnregisterReceiver(void *);

void Shadow_Destroy(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void Collider_Remove(void *);

void SceneActorRegistry_UnlinkActor(void *);

u32 Bee_Destroy(struct ChaseActor *actor)
{
    u8 *p = actor->base.drawable;
    u8 *q;
    u32 m;
    u32 v;
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
    v = actor->base.statusFlags & m;
    q = (void *)&actor->base.body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&actor->base.colliderA);
    SceneActorRegistry_UnlinkActor(actor);
    return 0;
}

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolveDamage(void);

void SceneActorPlacement_PlayResponseAndResetState(void);

void SceneActorReaction_SinkAndRemove(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_08605DB8[];

extern SceneActorCommandHandler const gUnk_086108B0[];

static inline void MonsterInit_InitializeBody(struct ChaseActor *arg0)
{
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    ZeroFill8(&origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->base.tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
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
    arg0->base.hudKind = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->base.hudLabel = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, arg0->base.drawableKind ? 0 : arg0->base.drawable);
    SceneBody_SetMovementProbe(body, ((struct Bee *)arg0)->base.movementProbe, 0x40, 0x40);
}

static inline void Bee_ReadPalette(struct ChaseActor *actor, u16 base, u16 alternate)
{
    s32 r1v;
    actor->base.paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0xF4);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->graphics, r1v);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->palette =
            gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
            SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct ChaseActor *actor, void *cb0, void *cb1,
                                              void *cb2, void *cb3, void *cb4, void *hidden,
                                              void *cb5, void *cb6, void *defaultReaction,
                                              void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.onUpdate = cb0;
    MonsterInit_WriteCallback(&actor->base.animationQuery.opaque, cb1);
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

static inline void Bee_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Bee_OnPlayerInteraction;
    actor->base.destroy = (SceneActorResultCallback)Bee_Destroy;
}

static inline void Bee_InstallRoutes(struct ChaseActor *actor)
{
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_08605DB8);
    actor->base.routeReset = Bee_ResetRoute;
    actor->base.reactionGate = Bee_SetTrackingReactionFlag;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, Bee_OnDetectionCheck);
}

s32 Bee_Init(void *object)
{
    struct Bee *bee = object;
    struct ChaseActor *actor = object;
    struct ContactPosition tmp;
    u32 flag;
    s32 r1v;
    bee->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_BEE);
    Bee_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x9506, 0x62C7);
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
        res = ScriptProc_Start(0xCB05, 0x9506);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked((void *)&((struct SceneActorAnimationTileDrawable *)heap)->tile, heap, 0,
                          0, 2, 2, 0x3C, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor);
    if (Script_SeekToKeyword('P')) {
        bee->behaviorDelay = MonsterInit_ReadByte(0x68);
        bee->behaviorCooldown = 0;
        bee->duration = MonsterInit_ReadHalfword(100);
    }
    if (Script_SeekToKeyword(0x70)) {
        struct ContactPosition position;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        position = actor->base.body.position;
        if (MonsterInit_GetDirection(actor) == 0)
            position.x += 0x80;
        else
            position.z += 0x80;
        bee->initialPosition = position;
        actor->base.terrainCell = MonsterInit_CellId(&bee->initialPosition);
        {
            s32 groundHeight =
                MonsterInit_TerrainHeight(actor->base.terrainCell, &bee->initialPosition);
            s32 offset;
            u16 *ground = &actor->base.groundHeight;
            *ground = groundHeight;
            offset = (u16)actor->base.body.position.y - groundHeight;
            actor->base.heightOffset = offset;
            bee->initialHeight = offset;
            actor->base.body.position.y = *ground;
            bee->previousGround = *ground;
        }
    }
    bee->anchor = 0;
    actor->base.colliderA.responseValue = Script_GetKeywordValue('n', 0);
    Bee_ReadPalette(actor, 0xF4, 0x192);

    Bee_InitColliders(actor);
    MonsterInit_InstallPrimary(
        actor, Bee_OnUpdate, Bee_GetMovementAnimationCode, 0, Bee_UpdateTurnAnimation,
        Bee_ResetIdleAnimation, 0, Bee_UpdateMovementStop, Bee_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, 0, SceneActorReaction_ResolveDamage,
        SceneActorPlacement_PlayResponseAndResetState);
    Bee_InstallRoutes(actor);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
    MonsterInit_SetMotionFlags(actor, 0x60);
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
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                  actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    MonsterInit_SetState(actor, 0, 2, 0);
    MonsterInit_SetControl(actor, actor->base.onAlternateEntry, 1);
    SceneActorBehavior_InstallCallbackTables(actor);
    Bee_InitActionPreset(actor);
    Bee_InitBehaviorHandlers(actor);
    Bee_InitReactionHandlers(actor);
    actor->base.field2E8 = 2;
    MonsterInit_Drawable(actor)[7] = 1;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Bee_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

void Bee_Create(void)
{
    void *m = Heap_Alloc(0x490);
    if (m != 0) {
        Memory_Clear(m, 0x490);
        if (Bee_Init(m) < 0) {
            Bee_Destroy(m);
            Heap_Free(m);
        }
    }
}
