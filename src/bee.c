#include "bee.h"

#include "global.h"

#include "scene_actor_control_phase.h"

#include "scene_actor_animation.h"

#include "player.h"

#include "scene_actor_behavior.h"

#include "script_utility.h"

#include "scene_actor_terrain.h"

#include "engine.h"

#include "ram.h"

#include "scene_actor_control_motion.h"

#include "display_sequence_inline.h"

#include "contact_actor.h"

#include "movement_curve.h"

#include "scene_actor_reset.h"

#include "scene_actor_state.h"

#include "scene_actor_collision.h"

#include "scene_body_api.h"

#include "render.h"

#include "engine_globals.h"

#include "scene_actor_init_group.h"

#include "scene_actor_init_graphics.h"

void Bee_OnAttackCollision(void)
{
}

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);

static inline u8 Bee_ContactUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (!(actor->flags2D4 & mask)) {
            u32 mask = 0x20000;
            if (!(actor->flags2D0 & mask) && actor->health > 0)
                return 0;
        }
        return 1;
    }
    return 0;
}

void Bee_ApplyContactReaction(struct SceneActorBehaviorContactView *source,
                              struct SceneActorBehaviorContactView *destination,
                              struct SceneActorRegistryResetView *actor)
{
    u32 index;
    s32 power;
    u32 sample;
    if (Bee_ContactUnavailable(actor))
        return;
    {
        u32 mask = 0x800;
        if (source->flags & mask)
            return;
    }
    SceneActorBehavior_ApplyContactReaction(source, destination, (void *)actor);
    index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    sample = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    {
        s32 old = destination->power;
        u32 mask = 15;
        power = old - (sample & mask);
    }
    power = power < 20 ? 20 : power;
    destination->power = power;
    {
        struct SceneActorControlState *control = SceneActorControl_GetState(actor);
        u32 *flags = &control->flags3D4;
        u32 mask = ~4;
        *flags &= mask;
    }
}

static inline u8 Bee_TakeControlEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline void Bee_SelectAnimation(struct SceneActorAnimationView *actor, u32 selection,
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

void Bee_UpdateTurnAnimation(struct SceneActorAnimationView *actor)
{
    struct Bee *body = (void *)actor->link;
    u32 variant, flags;
    if (Bee_TakeControlEntry(&actor->directionalEntryFlag)) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        Bee_GetDirectionalFrameAndFlip(actor->direction, &variant, &flags);
        {
            struct DisplaySequenceState *sequence =
                &((struct BeeDrawable *)actor->drawable)->sequence;
            sequence->frameCount = 0;
        }
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        {
            u32 direction = actor->direction;
            body->savedFacing = direction;
        }
    } else if (body->savedFacing != actor->direction) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        struct DisplaySequenceState *sequence = &((struct BeeDrawable *)actor->drawable)->sequence;
        u32 frame = sequence->index, time = sequence->elapsed;
        Bee_GetDirectionalFrameAndFlip(actor->direction, &variant, &flags);
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        sequence->index = frame;
        sequence->elapsed = time;
        {
            u32 direction = actor->direction;
            body->savedFacing = direction;
        }
    } else
        actor->restart = 0;
}

void Bee_ResetIdleAnimation(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
}

void Bee_ResetReactionAnimation(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
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

void Bee_UpdateRecoveryAnimation(struct SceneActorRegistryResetView *actor)
{
    if (Bee_TakeReadyFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    if (actor->field2B3)
        actor->field2B0 = 1;
}

// confidence=low: preserves sequence timing when the Bee changes facing without entering a state.

void Bee_UpdateFacingAnimation(struct SceneActorAnimationView *actor)
{
    struct Bee *body = (void *)actor->link;
    u32 variant, flags;
    if (Bee_TakeControlEntry(&actor->directionalEntryFlag)) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        Bee_GetDirectionalFrameAndFlip(actor->facingAngle, &variant, &flags);
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        SceneActorAnimation_SelectDirectionalEntry(actor, 1);
        {
            u32 direction = actor->facingAngle;
            body->savedFacing = direction;
        }
    } else if (body->savedFacing != actor->facingAngle) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        struct DisplaySequenceState *sequence = &((struct BeeDrawable *)actor->drawable)->sequence;
        u32 frame = sequence->index, time = sequence->elapsed;
        Bee_GetDirectionalFrameAndFlip(actor->facingAngle, &variant, &flags);
        Bee_SelectAnimation(actor, entry->value, variant, flags);
        sequence->index = frame;
        sequence->elapsed = time;
        {
            u32 direction = actor->facingAngle;
            body->savedFacing = direction;
        }
    } else
        actor->restart = 0;
}

void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);

void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *, struct SceneVec *);

static inline s32 Bee_QuarterMotion(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}

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

static inline void Bee_SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

// confidence=low: eases saved motion before returning to idle.

void Bee_UpdateMovementStop(struct SceneActorRegistryResetView *actor)
{
    struct Bee *body = actor->context;
    s32 x, z;
    s16 *priorX, *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u16 *flags;
        {
            u32 value = 0x20;
            flags = &actor->flags2D4;
            value |= *flags;
            *flags = value;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
    }
    {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
    }
    {
        s32 current = actor->reactionStart.x;
        s16 *field = &actor->reactionEnd.x;
        s32 difference = current - *field;
        priorX = field;
        x = Bee_QuarterMotion(difference);
    }
    {
        s32 current = actor->reactionStart.z;
        s16 *field = &actor->reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = Bee_QuarterMotion(difference);
    }
    x = Bee_ClampMotion(x);
    z = Bee_ClampMotion(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (Bee_IsMotionAtRest(x, z)) {
        actor->field2B0 = 1;
        {

            struct BeeSignedFlag *f = (void *)&actor->flags2D4;
            s32 m = ~0x20;
            m &= (u16)f->bits;
            f->bits = m;
        }
        {
            u32 *f = &actor->flags2CC;
            s32 m = ~0x80;
            *f &= m;
        }
        Bee_DisableMovement(actor);
        Bee_SetState(actor, 0, 2, 0);
        {
            u32 mask = 8;
            u32 *flags = &body->flags;
            *flags |= mask;
        }
        body->routeCooldown = 60;
    }
}

/* SceneActorControl_CompleteScriptedResponse_0809F368: SceneActorControl_CompleteScriptedResponse with other constants (ported by relatives.py, fixed by hand; provisional name) */

void *BestiaryDisplay_GetEntryForActor(u32, u32);

void GameState_SetEventFlag(void *);

void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);

void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void Bee_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
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
    Bee_SetState(actor, 1, 0, 2);
    ActorShadow_Disable(actor->shadow);
}

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

s32 Div(s32, s32);

void CpuSet(const void *, void *, u32);

static inline s32 Bee_TruncateShift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline u32 Bee_ReturnRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}

static inline void Bee_ClearReturnMotion(u32 *zero, void *out)
{
    *zero = 0;
    CpuSet(zero, out, 0x05000002);
}

static inline void Bee_EnterState(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

// confidence=low: initializes a small random movement and returns to the player when it stops.

void Bee_ReturnToPlayerWhenStopped(struct SceneActorRegistryResetView *actor)
{
    u32 zero;
    s32 length;
    struct ContactPosition *motion;
    if (Bee_TakeControlEntry(&actor->field2B1)) {
        struct Bee *body = actor->context;
        u32 angle;
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        Bee_ClearReturnMotion(&zero, &body->motion);
        angle = (u8)Bee_ReturnRandom();
        {
            const s16 *table = gUnk_086149C4;
            s32 vx = Bee_TruncateShift(table[angle], 6);
            actor->secondaryMotion.x = vx;
        }
        {
            const s16 *table = gUnk_086149C4;
            u32 index = angle + 64;
            u32 mask = 255;
            s32 vz;
            index &= mask;
            vz = Bee_TruncateShift(table[index], 6);
            actor->secondaryMotion.z = vz;
        }
    }
    motion = &actor->secondaryMotion;
    length = motion->x * motion->x + motion->z * motion->z;
    if (!length) {
        Bee_DisableMovement(actor);
        Bee_ClearReturnMotion(&zero, (u8 *)motion);
        Bee_BeginMoveToPosition(actor,
                                &((struct SceneActorChasePlayer *)gPlayers[0])->target.position);
        Bee_EnterState(actor, 2, 0);
    }
    actor->counter2AC++;
}

extern void *gUnk_08605D64[];

void Bee_InitActionPreset(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = gUnk_08605D64;
}

u8 *SceneBody_GetById(int);

static inline u32 Bee_AnchorRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + 0x0203B400);
}

static inline u32 Bee_AnchorActorFlags(struct SceneActorRegistryResetView *actor, u32 input,
                                       u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->flags2D4 & mask : actor->flags2D0 & mask;
}

static inline u8 Bee_AnchorUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor && (Bee_AnchorActorFlags(actor, 0x40, 0x2D4) ||
                  Bee_AnchorActorFlags(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}

static inline void Bee_FindDelta(struct ContactPosition *d, const struct ContactPosition *a,
                                 const struct ContactPosition *b)
{
    d->x = b->x - a->x;
    d->y = b->y - a->y;
    d->z = b->z - a->z;
}

static inline void Bee_AnchorPolarOffset(struct ContactPosition *out, u16 angle, s32 radius)
{
    out->x = Bee_TruncateShift(gUnk_086149C4[(angle + 0x40) & 255] * radius, 12);
    out->y = 0;
    out->z = Bee_TruncateShift(gUnk_086149C4[angle & 255] * radius, 12);
}

static inline void Bee_SetAnchorControl(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    u8 *taken = &a->field2B2;
    zero = 0;
    *taken = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}

static inline void Bee_ClearAnchorMotion(u32 *zero, void *out)
{
    *zero = 0;
    CpuSet(zero, out, 0x05000002);
}

static inline void Bee_SetAnchorState(struct SceneActorRegistryResetView *a, u32 state, u32 phase,
                                      u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
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

void Bee_StartMoveToBody(struct SceneActorRegistryResetView *actor, u16 *args)
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
    body = actor->context;
    {
        u16 value = args[1];
        body->bodyId = value;
    }
    body->anchor = (void *)SceneBody_GetById(body->bodyId);
    parameter = args[2];
    body->polarSpeed = parameter;
    body->index = 0;
    Bee_FindDelta(&work.delta, &actor->body.position, &body->anchor->position);
    actor->tracking.heading = Bee_GetBearing(&work.delta);
    angle = ((Bee_AnchorRandom() & 0x3F) + actor->tracking.heading) & 0xFF;
    Bee_AnchorPolarOffset(&offset, angle, 0x200);
    body->remaining = 0x500;
    Bee_AddTargetOffset(&offset, &actor->body.position);
    Bee_BuildMovementCurve(actor, &offset);
    Bee_SubVector(actor, &actor->body.position, &offset);
    Bee_ClearAnchorMotion(&work.fill, &body->motion);
    Bee_SetAnchorState(actor, 2, 0, 10);
    Bee_SetAnchorControl(actor, actor->entryTarget228, 1);
}

u8 *TerrainModifiers_FindCellOverride(int, int);

static inline u8 *Bee_GetTerrainProperties(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}

static inline void Bee_ClearResetVector(u32 *fillp, u32 value, void *out)
{
    *fillp = value;
    CpuSet(fillp, out, 0x05000002);
}

void Bee_ResetRoute(struct SceneActorRegistryResetView *actor)
{
    u32 zero4;
    u8 *bytes;
    struct Bee *context = actor->context;
    struct ContactPosition *source;
    u32 zero;
    s32 height, slope;
    u8 *c;
    u32 cellIndex;
    u8 zero8;
    u32 fill;
    u8 *first;
    first = &context->routeReset;
    zero = 0;
    zero8 = 0;
    *first = zero;
    context->curveTime = zero;
    context->flightPhase = zero8;
    context->curveActive = 1;
    context->curve = zero;
    context->curveDirection = zero8;
    context->curveMode = zero8;
    context->flags = zero;
    source = &context->initialPosition;
    actor->body.position = *source;
    {
        s32 x = source->x >> 8, z = source->z >> 8, index;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
        actor->terrainCell = index;
        cellIndex = actor->terrainCell;
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
    actor->groundHeight2BE = height;
    actor->field2EA = actor->body.position.y - height;
    actor->body.position.y = actor->groundHeight2BE;
    if (!actor->drawableKind) {
        *(struct BeePackedPosition *)&((struct BeeDrawable *)actor->drawable)->position =
            *(struct BeePackedPosition *)&actor->body.position;
    } else {
        *(struct BeePackedPosition *)&((struct SceneActorRegistryTileDrawable *)actor->drawable)
             ->position = *(struct BeePackedPosition *)&actor->body.position;
    }
    {
        void *dst = &context->start;
        zero4 = 0;
        Bee_ClearResetVector(&fill, zero4, dst);
    }
    {
        void *dst = &context->end;
        Bee_ClearResetVector(&fill, zero4, dst);
    }
    {
        void *dst = &context->motion;
        Bee_ClearResetVector(&fill, zero4, dst);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    DisplaySequence_Step(actor->drawable);
}

static inline u32 Bee_FlightActorFlags(struct SceneActorRegistryResetView *actor, u32 input,
                                       u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->flags2D4 & mask : actor->flags2D0 & mask;
}

static inline u8 Bee_FlightUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor && (Bee_FlightActorFlags(actor, 0x40, 0x2D4) ||
                  Bee_FlightActorFlags(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}

static inline u8 Bee_TestFlightFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline s32 Bee_ScaleSine(s32 value, u32 shift)
{
    return value >= 0 ? value >> shift : -((-value) >> shift);
}

void Bee_UpdateFlightHeight(struct SceneActorRegistryResetView *actor)
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
    g = &actor->groundHeight2BE;
    {
        u32 diff = *g - state->previousGround;
        actor->field2EA -= diff;
        state->previousGround = *g;
    }
    if (!Bee_FlightActorFlags(actor, 0x100000, 0x2D0)) {
        actor->body.position.y = *g + actor->field2EA;
        return;
    }
    if ((s16)actor->field2EA <= 259) {
        {
            u32 mask = 1;
            if (Bee_TestFlightFlags(state->flags, mask)) {
                actor->body.position.y = actor->groundHeight2BE;
                return;
            }
        }
        actor->field2EA += Bee_ScaleSine(gUnk_086149C4[state->flightPhase], 9);
        if (state->flightPhase <= 63)
            state->flightPhase++;
        actor->body.position.y = actor->groundHeight2BE + actor->field2EA;
    }
    {
        s32 currentGround = actor->groundHeight2BE;
        s32 raisedY = actor->body.position.y - 260;
        delta = currentGround - raisedY;
    }
    absolute = delta >= 0 ? delta : -delta;
    if (absolute > 16) {
        if (delta < 0)
            actor->field2EA -= 8;
        else
            actor->field2EA += 8;
    }
    actor->body.position.y = actor->groundHeight2BE + actor->field2EA;
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
        drawable->position.y -= Bee_ScaleSine(gUnk_086149C4[state->flightPhase], 5);
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

static inline u8 Bee_TestMovementFlags(u32 flags, u32 mask)
{
    if (flags & mask)
        return TRUE;
    return FALSE;
}

static inline u32 Bee_ReadMovementCode(const u32 *t, u32 i)
{
    return t[i];
}

u32 Bee_GetMovementAnimationCode(struct SceneActorRegistryResetView *actor)
{
    struct Bee *state = actor->context;
    s32 sample;
    s32 index;
    sample = Bee_QuantizeCurveTime(state->curveTime);
    if (sample < 0)
        sample = -sample;
    index = sample;
    if (index > 7)
        index = 7;
    {
        u32 mask = 4;
        u32 flags = state->flags;
        if (Bee_TestMovementFlags(flags, mask))
            return Bee_ReadMovementCode(gUnk_08605D68, index) + 6;
        return Bee_ReadMovementCode(gUnk_08605D68, index);
    }
}

void Bee_SetMovementEndpoints(struct SceneActorRegistryResetView *actor,
                              struct ContactPosition *target)
{
    struct Bee *context = actor->context;
    context->start = actor->body.position;
    context->end = *target;
    context->curveTime = 0;
}

u8 Bee_AdvanceMovementCurve(struct SceneActorRegistryResetView *actor)
{
    struct Bee *ctx = actor->context;
    struct ContactPosition start, end;
    start = ctx->start;
    end = ctx->end;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
        *(u32 *)&end = mask & *(u32 *)&end;
    }
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

void Bee_InitProfile(struct SceneActorRegistryResetView *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(0x14, 0);
}

void Bee_DisableMovement(struct SceneActorRegistryResetView *actor)
{
    struct Bee *context = actor->context;
    u32 m = 0x100000;
    u32 *p = &actor->flags2D0;
    u32 *p2;
    u32 m2;
    *p = *p | m;
    p2 = &context->flags;
    m2 = ~3;
    *p2 = *p2 & m2;
    actor->colliderB.flags &= ~4;
}

static inline void Bee_FillMotion(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

void Bee_BeginMoveToPosition(struct SceneActorRegistryResetView *actor,
                             struct ContactPosition *target)
{
    struct Bee *context = actor->context;
    u32 stack[3];
    s16 *a;
    s16 *b;
    Bee_SetMovementEndpoints(actor, target);
    Bee_AdvanceMovementCurve(actor);
    {
        u32 one = 1;
        u8 *q = &actor->state2A8;
        u32 zero = 0;
        *q = zero;
        {
            actor->phase2A9 = zero;
            actor->behavior2AA = one;
            actor->counter2AC = zero;
        }
        actor->field2B1 = one;
        b = (s16 *)(stack + 1);
        a = (void *)&actor->body.position;
        b[0] = target->x - a[0];
        b[1] = target->y - a[1];
        b[2] = target->z - a[2];
        actor->unknown3CD[0] = Math_Atan2(b[0], b[2]);
        Bee_FillMotion(&context->motion, stack, zero);
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

void Bee_TickBehaviorCooldown(struct SceneActorRegistryResetView *actor)
{
    struct Bee *context = actor->context;
    u8 v = context->behaviorCooldown;
    if (v != 0)
        context->behaviorCooldown = v - 1;
}

static inline s32 Bee_Shift10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}

static inline void Bee_SubtractPoints(struct ContactPosition *out, struct ContactPosition *a,
                                      struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline void Bee_InterpolatePoint(struct ContactPosition *out, struct ContactPosition *start,
                                        struct ContactPosition *end, s32 tx, s32 tz, s32 t)
{
    struct ContactPosition offset;
    struct ContactPosition acc;
    s32 q, p, twiceP, thriceQ, tangent, h11;
    s32 scaled, value;
    Bee_SubtractPoints(&offset, end, start);
    ZeroFill8(&acc);
    q = Bee_Shift10(t * t);
    p = Bee_Shift10(q * t);
    twiceP = p * 2;
    thriceQ = q * 3;
    tangent = (p - 2 * q + t) * tz;
    h11 = p - q;
    scaled = ((twiceP - thriceQ + 1) * acc.x) * tx;
    value = (thriceQ - twiceP) * offset.x;
    value = scaled + value + tangent + h11;
    out->x = start->x + Bee_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.y) * tx;
    value = (-twiceP + thriceQ) * offset.y;
    value = scaled + value + tangent + h11;
    out->y = start->y + Bee_Shift10(value);
    scaled = ((twiceP - thriceQ + 1) * acc.z) * tx;
    value = (-twiceP + thriceQ) * offset.z;
    value = scaled + value + tangent + h11;
    out->z = start->z + Bee_Shift10(value);
}

void Bee_BuildMovementCurve(struct SceneActorRegistryResetView *actor,
                            struct ContactPosition *target)
{
    struct Bee *route = actor->context;
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

void Collider_SetAttackProperties(void *, u16, u16, u32, u32, u16);

static inline u32 Bee_AttackActorFlags(struct SceneActorRegistryActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->flags2D4 & mask : actor->flags2D0 & mask;
}

static inline u8 Bee_AttackUnavailable(struct SceneActorRegistryActor *actor)
{
    if (actor && (Bee_AttackActorFlags(actor, 0x40, 0x2D4) ||
                  Bee_AttackActorFlags(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}

void SceneActorCollision_FireSecondaryAttack(struct SceneActorRegistryActor *actor)
{
    if (Bee_AttackUnavailable(actor))
        return;
    {
        u16 mask = 0x100;
        if (mask & actor->flags2D4) {
            struct ContactCollider *collider =
                &((struct SceneActorRegistryResetView *)actor)->colliderB;
            u16 field3C = ((struct SceneActorRegistryResetView *)actor)->attacks.first[1];
            u16 field40 = ((struct SceneActorRegistryResetView *)actor)->attacks.second[1];
            u32 reactionFlags = ((struct SceneActorRegistryResetView *)actor)->attacks.flags[1];
            u16 field44 = ((struct SceneActorRegistryResetView *)actor)->attacks.parameter[1];
            Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
            return;
        }
    }
    {
        struct ContactCollider *collider =
            &((struct SceneActorRegistryResetView *)actor)->colliderB;
        u16 field3C = ((struct SceneActorRegistryResetView *)actor)->attacks.first[0];
        u16 field40 = ((struct SceneActorRegistryResetView *)actor)->attacks.second[0];
        u32 reactionFlags = ((struct SceneActorRegistryResetView *)actor)->attacks.flags[0];
        u16 field44 = ((struct SceneActorRegistryResetView *)actor)->attacks.parameter[0];
        Collider_SetAttackProperties(collider, field3C, field40, 0, reactionFlags, field44);
    }
}

static inline u8 Bee_HasDetection(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *p = &actor->tracking;
    u32 mask = 2;
    if (p->flags & mask)
        return 1;
    return 0;
}

u32 Bee_OnDetectionCheck(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *p;
    s32 distance;
    if (!Bee_HasDetection(actor))
        goto zero;
    p = &actor->tracking;
    {
        s32 mask = -3;
        mask &= p->flags;
        p->flags = mask;
    }
    distance =
        actor->body.position.y - ((struct SceneActorChasePlayer *)gPlayers[0])->target.position.y;
    if (distance < 0)
        distance = -distance;
    if (distance > 0x200)
        goto zero;
    {
        u32 *a = &actor->tracking.distance;
        u32 *b = &actor->tracking.limit;
        if (*a >= *b)
            goto zero;
    }
    goto one;
zero:
    return 0;
one: {
    u32 mask = 0x80;
    u16 *flags = &actor->flags2D6;
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

void Slime_SetStateFlag(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *tracking = &actor->tracking;
    u8 m = 8;
    tracking->flags = m | tracking->flags;
}

u32 Bee_GetMovementVariant(struct SceneActorRegistryResetView *actor)
{
    u32 r;
    switch (actor->paletteOffset) {
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

u32 Bee_UpdateGroundHeight(struct SceneActorRegistryResetView *actor)
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

static inline u8 Bee_TakeEntryFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline u8 Bee_TestRouteFlags(struct Bee *body, u32 mask)
{
    u32 flags = body->flags;
    if (flags & mask)
        return 1;
    return 0;
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
    out->x = Bee_TruncateShift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Bee_TruncateShift(gUnk_086149C4[angle & 255] * speed, 12);
}

static inline void Bee_SubtractPositions(struct ContactPosition *out,
                                         const struct ContactPosition *from,
                                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline void Bee_AddPosition(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = out->x + from->x;
    out->y = out->y + from->y;
    out->z = out->z + from->z;
}

static inline void Bee_InstallControl(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    u8 *taken = &a->field2B2;
    zero = 0;
    *taken = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}

static inline u8 Bee_GetHeading(const struct SceneActorRegistryResetView *actor)
{
    return actor->tracking.heading;
}

// confidence=low: follows a randomized route and returns to the Bee behavior selector.

void Bee_FollowWanderRoute(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition dest;
    struct ContactPosition delta;
    struct Bee *body = actor->context;
    if (Bee_TakeEntryFlag(&actor->field2B1)) {
        Bee_InstallControl(actor, actor->entryTarget228, 1);
        body->index = 0;
        if (Bee_TestRouteFlags(body, 8)) {
            {
                u8 reverseBias = 128;
                Bee_MakePolarVector(
                    &dest,
                    (actor->tracking.heading + ((Bee_NextRandom() & 63) + reverseBias)) & 255, 320);
            }
            Bee_ClearRouteFlags(body, 8);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                s32 duration = 6 << variant;
                body->remaining = duration;
            }
        } else {
            Bee_MakePolarVector(
                &dest, Bee_OffsetHeading(Bee_NextRandom(), &actor->tracking.heading) & 255, 512);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                s32 duration = 40 << variant;
                body->remaining = duration;
            }
        }
        Bee_AddPosition(&dest, &actor->body.position);
        Bee_BuildMovementCurve(actor, &dest);
        Bee_SubVector(actor, &actor->body.position, &dest);
        ZeroFill8(&body->motion);
    }
    Bee_SubtractPositions(&delta, &actor->body.position, &body->points[body->index]);
    {
        u8 facing = Bee_GetHeading(actor);
        actor->body.direction = facing;
    }
    if ((body->remaining > 0) && (((delta.x * delta.x) + (delta.z * delta.z)) > 1024)) {
        s32 steps = Bee_Max(Bee_TruncateShift(body->remaining, 5), 1);
        actor->body.motion.x += Div(body->points[body->index].x - actor->body.position.x, steps);
        actor->body.motion.z += Div(body->points[body->index].z - actor->body.position.z, steps);
        body->motion = actor->body.motion;
        body->remaining -= actor->counter2AC;
    } else if ((++body->index) > 8) {
        actor->body.position.x = body->points[8].x;
        actor->body.position.z = body->points[8].z;
        Bee_MakePolarVector(&dest, ((Bee_NextRandom() & 63) + actor->tracking.heading) & 255, 512);
        Bee_AddPosition(&dest, &actor->body.position);
        Bee_BuildMovementCurve(actor, &dest);
        Bee_SubVector(actor, &actor->body.position, &dest);
        {
            s32 variant = Bee_GetMovementVariant(actor);
            s32 duration = (body->duration) << variant;
            body->remaining = duration;
        }
        body->index = 0;
        actor->counter2AC = 0;
        return;
    } else {
        actor->body.position.x = Bee_GetWaypointX(body, body->index - 1);
        actor->body.position.z = Bee_GetWaypointZ(body, body->index - 1);
    }
    if (((s32)actor->counter2AC) <= 19)
        actor->counter2AC++;
    if (body->routeCooldown)
        body->routeCooldown--;
    else if (actor->tracking.distance <= 0xFFFE) {
        if (body->behaviorCooldown)
            Bee_EnterState(actor, 2, 1);
        else
            Bee_EnterState(actor, 2, 2);
    }
}

void SceneActorControl_TickPendingTarget(struct SceneActorRegistryResetView *actor)
{
    struct Bee *context = actor->context;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        actor->body.direction = actor->tracking.heading;
        {
            void *target = actor->entryTarget228;
            u32 one = 1;
            actor->field2B2 = one;
            actor->field2B0 = 0;
            actor->field2B5 = one;
            actor->target = target;
            actor->field2C8 = 0;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = 0;
        }
    }
    {
        u32 *counter = &actor->counter2AC;
        if ((s32)*counter > 30) {
            u32 phase = 2;
            actor->state2A8 = 0;
            (*(&actor))->phase2A9 = phase;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            {
                u32 mask = 8;
                u32 *flags = &context->flags;
                *flags |= mask;
            }
            context->routeCooldown = 60;
        } else {
            *counter = *counter + 1;
        }
    }
}

static inline void Bee_ResetBehavior(struct SceneActorRegistryResetView *actor, u32 phase,
                                     u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Bee_UpdateReactionPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        actor->body.direction = actor->tracking.heading;
        {
            void *callback = (void *)Bee_ResetReactionAnimation;
            u32 kind = 0x25;
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
    }
    if ((s32)actor->counter2AC > 0x28 && actor->field2B3)
        Bee_ResetBehavior(actor, 2, 3);
    actor->counter2AC++;
}

static inline void Bee_SetRecoveryControl(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero = 0;
    a->field2B2 = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}

// confidence=low: approaches a saved player position during the recovery animation.

void Bee_ApproachSavedPlayerPosition(struct SceneActorRegistryResetView *actor)
{
    struct Bee *body = actor->context;
    struct SceneActorBody *motion;
    s32 speed;
    struct SceneActorMotionSubject *saved;
    s16 dx, dz;
    if (Bee_TakeControlEntry(&actor->field2B1)) {
        actor->body.direction = actor->tracking.heading;
        Bee_SetRecoveryControl(actor, Bee_UpdateRecoveryAnimation, 0x26);
        *(struct BeePackedPosition *)&body->curveSubject.position =
            *(struct BeePackedPosition *)&((struct SceneActorChasePlayer *)gPlayers[0])
                 ->target.position;
        {
            u32 value = body->behaviorDelay;
            body->behaviorCooldown = value;
        }
        {
            u32 mask = 0x100;
            actor->flags2D4 = mask | actor->flags2D4;
        }
        Sound_PlaySong(0x3C8);
    }
    saved = &body->curveSubject;
    motion = &actor->body;
    speed = 24;
    dx = saved->position.x - motion->position.x;
    dz = saved->position.z - motion->position.z;
    if ((dx < 0 ? -dx : dx) > 32 || (dz < 0 ? -dz : dz) > 32) {
        if (dx || dz) {
            motion->direction = Math_Atan2(dx, dz);
            motion->motion.x +=
                Bee_TruncateShift(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
            motion->motion.z += Bee_TruncateShift(gUnk_086149C4[motion->direction] * speed, 12);
        }
    }
    if (actor->field2B0)
        Bee_EnterState(actor, 2, 4);
    else
        actor->counter2AC++;
}

static inline u32 Bee_PauseRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR);
}

static inline u16 Bee_MaskFlags(u16 *p, u32 m)
{
    return *p & m;
}

static inline void Bee_PausePolarMotion(struct ContactPosition *m, u32 angle, s32 radius)
{
    m->x = Bee_TruncateShift(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius, 12);
    m->y = 0;
    m->z = Bee_TruncateShift(gUnk_086149C4[angle] * radius, 12);
}

void Bee_UpdateWanderPause(struct SceneActorRegistryResetView *actor)
{
    s32 cnt;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = (void *)Bee_UpdateFacingAnimation;
        u32 kind = 0x27;
        u32 zero;
        u32 angle;
        u32 r;
        u8 *fp;
        u8 *taken = &actor->field2B2;
        zero = 0;
        *taken = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
        r = Bee_PauseRandom();
        fp = &actor->tracking.heading;
        angle = (r & 0x1F) + 0x71;
        angle = (angle + *fp) & 0xFF;
        Bee_PausePolarMotion(&actor->secondaryMotion, angle, 0x20);
    }
    cnt = actor->counter2AC;
    if (cnt > 0x1E) {
        actor->flags2D4 = Bee_MaskFlags(&actor->flags2D4, ~0x100);
        Bee_SetState(actor, 0, 2, 0);
    } else {
        actor->counter2AC = cnt + 1;
    }
}

static inline s32 Bee_ScalePolarComponent(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline s32 Bee_ScaleDuration(s32 n)
{
    if (n >= 0)
        return n >> 5;
    return -((-n) >> 5);
}

static inline void Bee_MakeAnchorVector(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Bee_ScalePolarComponent(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Bee_ScalePolarComponent(gUnk_086149C4[angle & 255] * speed);
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

void Bee_FollowAnchorRoute(struct SceneActorRegistryResetView *actor)
{
    struct Bee *route = actor->context;
    struct ContactPosition delta, target, step;
    s32 n;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        Bee_InstallControl(actor, actor->entryTarget228, 1);
        route->index = 0;
        Bee_SubtractPositions(&delta, &actor->body.position, &route->anchor->position);
        actor->tracking.heading = Math_Atan2(delta.x, delta.z);
        Bee_MakeAnchorVector(
            &target,
            Bee_OffsetAnchorHeading(Bee_NextAnchorRandom(), &actor->tracking.heading) & 255,
            route->polarSpeed);
        route->remaining = 0x500;
        Bee_AddPosition(&target, &actor->body.position);
        Bee_BuildMovementCurve(actor, &target);
        Bee_SubVector(actor, &actor->body.position, &target);
        ZeroFill8(&route->motion);
    }
    Bee_SubtractPositions(&step, &actor->body.position, &route->points[route->index]);
    Bee_SubtractPositions(&delta, &actor->body.position, &route->anchor->position);
    {
        u32 heading = Math_Atan2(delta.x, delta.z);
        actor->tracking.heading = heading;
        actor->body.direction = heading;
    }
    if ((s16)route->remaining > 0 && step.x * step.x + step.z * step.z > 1024) {
        n = Bee_Max(Bee_ScaleDuration((s16)route->remaining), 1);
        actor->body.motion.x += Div(route->points[route->index].x - actor->body.position.x, n);
        actor->body.motion.z += Div(route->points[route->index].z - actor->body.position.z, n);
        route->motion = actor->body.motion;
        route->remaining -= actor->counter2AC;
    } else {
        route->index++;
        if (route->index > 8) {
            actor->body.position.x = route->points[8].x;
            actor->body.position.z = route->points[8].z;
            Bee_SubtractPositions(&delta, &actor->body.position, &route->anchor->position);
            actor->tracking.heading = Math_Atan2(delta.x, delta.z);
            Bee_MakeAnchorVector(
                &target,
                Bee_OffsetAnchorHeading(Bee_NextAnchorRandom(), &actor->tracking.heading) & 255,
                route->polarSpeed);
            Bee_AddPosition(&target, &actor->body.position);
            Bee_BuildMovementCurve(actor, &target);
            Bee_SubVector(actor, &actor->body.position, &target);
            {
                u32 duration = route->duration << 5;
                route->remaining = duration;
            }
            route->index = 0;
            actor->counter2AC = 0;
            return;
        }
        actor->body.position.x = Bee_GetPreviousX(route, route->index - 1);
        actor->body.position.z = Bee_GetPreviousZ(route, route->index - 1);
    }
    if ((s32)actor->counter2AC <= 19)
        actor->counter2AC++;
}

extern void *gUnk_08605D88[];

void Bee_InitBehaviorHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->behaviorCallbacks = gUnk_08605D88;
}

extern void *gUnk_08605D9C[];

void Bee_InitReactionHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks29C = gUnk_08605D9C;
}

void SceneActorControl_DispatchControl(u8 *);

u32 Bee_OnUpdate(struct SceneActorRegistryResetView *actor)
{
    u32 i;
    void **t;
    Bee_UpdateGroundHeight(actor);
    i = actor->state2A8;
    t = actor->callbacks27C;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    Bee_UpdateTimersAndContacts(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Bee_OnPlayerInteraction(struct SceneActorRegistryResetView *actor)
{
    actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
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

static inline u8 Bee_TestDestroyFlags(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

u32 Bee_Destroy(struct SceneActorRegistryResetView *actor)
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
    if (Bee_TestDestroyFlags(&actor->flags2C0, m))
        SceneMessages_UnregisterReceiver(&actor->unknown9[3]);
    {
        u8 *r = actor->shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = actor->flags2D0 & m;
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

void SceneActorReaction_SinkAndRemove(void);

void SceneActorScript_OnCommand(void);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_08605DB8[];

extern SceneActorCommandHandler const gUnk_086108B0[];

static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
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
    SceneBody_SetSprite(body, arg0->drawableKind ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, ((struct Bee *)arg0)->movementProbe, 0x40, 0x40);
}

static inline void Bee_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                   u16 alternate)
{
    s32 r1v;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0xF4);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(((struct BeeDrawable *)actor->drawable)->animation, r1v);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
            gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
            SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct SceneActorRegistryResetView *actor, void *cb0,
                                              void *cb1, void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->contextCallback = cb0;
    MonsterInit_WriteCallback(&actor->animationQuery.opaque, cb1);
    actor->entryTarget224 = cb2;
    actor->entryTarget228 = cb3;
    actor->savedTarget = cb4;
    actor->reactionTarget230 = cb5;
    actor->deathControl = cb6;
    actor->field220 = hidden;
    actor->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_086108B0);
    actor->callback248 = 0;
    actor->update26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->damageCallback = damage;
    actor->contactCallback = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Bee_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Bee_OnPlayerInteraction;
    actor->destroyCallback = Bee_Destroy;
}

static inline void Bee_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_08605DB8);
    actor->callback248 = Bee_ResetRoute;
    actor->field23C = Slime_SetStateFlag;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Bee_OnDetectionCheck);
}

s32 Bee_Init(void *object)
{
    struct Bee *bee = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 r1v;
    bee->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x14);
    Bee_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x9506, 0x62C7);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0x9506);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked((void *)&((struct SceneActorAnimationTileDrawable *)heap)->tile, heap, 0,
                          0, 2, 2, 0x3C, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    if (Script_SeekToKeyword('P')) {
        bee->behaviorDelay = MonsterInit_ReadByte(0x68);
        bee->behaviorCooldown = 0;
        bee->duration = MonsterInit_ReadHalfword(100);
    }
    if (Script_SeekToKeyword(0x70)) {
        struct ContactPosition position;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        position = actor->body.position;
        if (MonsterInit_GetDirection(actor) == 0)
            position.x += 0x80;
        else
            position.z += 0x80;
        bee->initialPosition = position;
        actor->terrainCell = MonsterInit_CellId(&bee->initialPosition);
        {
            s32 groundHeight = MonsterInit_TerrainHeight(actor->terrainCell, &bee->initialPosition);
            s32 offset;
            u16 *ground = &actor->groundHeight2BE;
            *ground = groundHeight;
            offset = (u16)actor->body.position.y - groundHeight;
            actor->field2EA = offset;
            bee->initialHeight = offset;
            actor->body.position.y = *ground;
            bee->previousGround = *ground;
        }
    }
    bee->anchor = 0;
    actor->colliderA.responseValue = Script_GetKeywordValue('n', 0);
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
    MonsterInit_SetState(actor, 0, 2, 0);
    MonsterInit_SetControl(actor, actor->entryTarget228, 1);
    SceneActorBehavior_InstallCallbackTables(actor);
    Bee_InitActionPreset(actor);
    Bee_InitBehaviorHandlers(actor);
    Bee_InitReactionHandlers(actor);
    actor->field2E8 = 2;
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
