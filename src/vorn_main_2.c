#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reaction.h"
#include "global.h"
#include "vorn.h"
#include "scene_actor_reset.h"
#include "engine.h"
#include "scene_actor_registry.h"
#include "rotation_phase.h"
#include "scene_vec.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_contact_response.h"
#include "scene_actor_placement.h"
#include "scene_body_api.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"

extern const u8 gUnk_0824E230[];

void SceneActorReaction_ResetResponseState(void *, const void *, u32);

u8 Vorn_SelectReachableEscapeCell(struct ChaseActor *, u32);

struct Vorn;

u8 SceneActor_DispatchContextAction(struct Vorn *, u32);

struct VornRetreatFlags {
    s16 bits : 16;
};

struct VornRetreatContext {
    u8 unknown0[0x6C4];
    u32 requests, nearDistance, farDistance;
    u16 timer, unknown6D2;
    struct VornRetreatFlags flags;
};

static inline void Vorn_AdjustContextFlags(struct VornRetreatContext *body, u16 set, u32 clear)
{
    u16 bit = set;
    struct VornRetreatFlags *flags = &body->flags;
    flags->bits |= bit;
    flags->bits &= ~clear;
}

static inline u32 Vorn_RandomSample(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}

static inline u32 Vorn_ScatterDirection(const u8 *heading, u32 sample)
{
    return (sample & 63) + *heading;
}

static inline u8 Vorn_TryTrackedCell(void *object)
{
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(object);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    u32 cell;
    struct ContactPosition target;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    cell = SceneActorChase_CellAt(&tracking->target->position);
    if (cell == actor->base.terrainCell ||
        SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[0])->flags,
                               0x20000)) {
        SceneActor_DispatchContextAction(actor->base.context, 0);
        return 0;
    }
    SceneActorChase_CellCenter(cell, &target);
    if (!SceneActorChase_TryRoute(object, group->route, &actor->base.body.position, &target)) {
        Callback_InvokeActor(actor, group->fallback);
        return 0;
    }
    return 1;
}

u32 Vorn_SelectRetreatRoute(struct ChaseActor *actor, u32 reset)
{
    struct VornRetreatContext *body = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    u8 old;
    if ((!reset) && body->timer)
        return 0;
    body->timer = 60;
    SceneActorChase_ClearActorFlags(actor, 0x10);
    Vorn_AdjustContextFlags(body, 0x10, 0x40);
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E230, 0x486);
    body->requests++;
    if (actor->base.tracking.distance < body->nearDistance) {
        old = actor->base.body.direction;
        actor->base.body.direction = actor->base.tracking.heading + 0x80;
        if (!Vorn_SelectReachableEscapeCell(actor, old))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    } else if (actor->base.tracking.distance > body->farDistance) {
        old = actor->base.body.direction;
        {
            u32 random = Vorn_RandomSample();
            actor->base.body.direction =
                Vorn_ScatterDirection(&actor->base.tracking.heading, random);
        }
        if (!actor->base.tracking.target)
            actor->base.tracking.target =
                &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        if ((!Vorn_TryTrackedCell(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    } else {
        old = actor->base.body.direction;
        {
            u32 random = Vorn_RandomSample();
            actor->base.body.direction =
                Vorn_ScatterDirection(&actor->base.tracking.heading, random);
        }
        if (!actor->base.tracking.target)
            actor->base.tracking.target =
                &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        if ((!Vorn_TryTrackedCell(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    }
    {
        u32 bit = 4;
        u32 *field = &actor->base.statusFlags;
        u32 value = *field;
        value |= bit;
        *field = value;
    }
    return 1;
}

#define gRandomTableIndex gRandomIndex

s32 SceneActorRegistry_CountLiving(void);

s32 SceneActorRegistry_CountLivingByCategory(s32);

void SceneActorReaction_UpdateFacingAttack(void);

static inline u8 Inactive(struct Vorn *p)
{
    if (p) {
        u32 mask = 0x40;
        u32 offset = 0x2D4;
        if (p->chase.base.motionFlags & mask)
            goto inactive;
        {
            u32 mask = 0x20000;
            offset -= 4;
            if (p->chase.base.statusFlags & mask)
                goto inactive;
        }
        if (p->chase.base.health > 0)
            goto active;
    inactive:
        return 1;
    }
active:
    return 0;
}

static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}

static inline s32 RandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(const u8 *)(gRandomTable + gRandomIndex);
}

static inline void Clear(u32 *p, u32 mask)
{
    *p &= ~mask;
}

inline static s32 RandomByte_TrySoloAction(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return (u8)((const u16 *)0x0203B400)[gRandomIndex];
}

void SceneActor_TrySoloAction(struct Vorn *p)
{
    struct Vorn *q = p;
    u8 zero;
    s32 t1;
    u32 timer;
    if (Inactive(p))
        return;
    {
        u32 mask = 0xE33F;
        if (p->chase.base.motionFlags & mask)
            return;
    }
    if (!q->field688)
        return;
    t1 = 4;
    if (!Test(&q->flags6D4, t1))
        return;
    if (SceneActorRegistry_CountLivingByCategory(0x19) > 1)
        return;
    if (SceneActorRegistry_CountLiving() > 1)
        return;
    timer = q->timer68D;
    if ((zero = timer) != 0)
        q->timer68D = timer - 1;
    else {
        q->timer68D = 0xF0;
        if (RandomByte_TrySoloAction() <= 0x9F)
            return;
        if (!q->field688)
            return;
        {
            u32 state = 3;
            p->chase.base.state2A8 = state;
        }
        p->chase.base.phase2A9 = zero;
        p->chase.base.behavior2AA = zero;
        p->chase.base.counter2AC = zero;
        p->chase.base.field2B1 = 1;
        {
            u32 one = 1;
            void *callback = SceneActorReaction_UpdateFacingAttack;
            u32 mode = 0x2A;
            p->chase.base.field2B2 = one;
            p->chase.base.stepDone = zero;
            p->chase.base.controlKind = mode;
            p->chase.base.target = callback;
        }
        p->chase.base.field2C8 = zero;
        Clear(&p->chase.base.pendingControlFlags, 1);
        p->chase.base.pendingControl = zero;
    }
}

void Vorn_ParseField688(struct Vorn *ctx)
{
    if (Script_SeekToKeyword(0x73) != 0) {
        s32 val1;

        if (Script_GetPc() != 0)
            val1 = Script_GetValue();
        else
            val1 = 0;
        ctx->field68C = val1;

        if ((u8)val1 == 1) {
            ctx->field688 = 0;
        } else {
            s32 *p = &ctx->field688;
            s32 val2;

            if (Script_GetPc() != 0)
                val2 = Script_GetValue();
            else
                val2 = 0;
            *p = val2;
        }
    }
}

void Vorn_RunPendingScript(u8 *p)
{
    u32 *q = (u32 *)(p + 0x688);
    if (*q != 0) {
        Script_Call(*q, 0);
        *q = 0;
    }
}

void Vorn_ParseField69A(struct Vorn *ctx)
{
    if (Script_SeekToKeyword(0x50) != 0) {
        ctx->frame69A = (Script_GetPc() != 0) ? Script_GetValue() : 300;
    } else {
        ctx->frame69A = 300;
    }
    ctx->field698 = ctx->frame69A;
}

void Vorn_SelectRouteAwayFromTarget(struct ChaseActor *actor);

void Vorn_UpdatePursuitWait(void);

void SceneBody_StartRoute(void *, void *, u32, u32, u32);

struct VornResponseFlags {
    s16 bits : 16;
};

static inline u32 Vorn_ActorFlags(struct Vorn *actor, u32 mask)
{
    return actor->chase.base.behaviorFlags & mask;
}

static inline u8 Vorn_TestContext(struct Vorn *context, u16 mask, struct VornResponseFlags **field)
{
    struct VornResponseFlags *flags = (void *)&context->flags6D4;
    u16 value = flags->bits;
    value &= mask;
    *field = flags;
    if (value)
        return 1;
    return 0;
}

static inline u8 Vorn_TestResponseFlags(struct VornResponseFlags *flags, u32 mask)
{
    u16 value = flags->bits;
    if (value & mask)
        return 1;
    return 0;
}

static inline void Vorn_SetResponseFlags(struct VornResponseFlags *flags, u16 bits)
{
    flags->bits |= bits;
}

static inline void Vorn_SetTimedMotion(struct Vorn *actor, void *callback, u32 duration)
{
    actor->chase.base.pendingControl = (u32)callback;
    actor->chase.base.pendingControlData = 0;
    actor->chase.base.pendingControlRelease = 0;
    actor->chase.base.pendingControlKind = duration;
    actor->chase.base.pendingControlFlags = 1;
}

static inline void Vorn_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags = bits | tracking->flags;
}

static inline void Vorn_ResetContext(struct Vorn *a)
{
    struct SceneActorChaseTracking *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    a->chase.base.field2C2 = 64;
    a->chase.group.common.gate.flags = 0;
    a->chase.group.common.gate.counter = 0;
    flags = &a->chase.base.motionFlags;
    {
        s32 mask = ~0x380;
        *flags = mask & (*flags);
    }
    motion = &a->chase.base.tracking;
    {
        s32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    control = &a->chase.group.common.control;
    if (Vorn_ActorFlags(a, 0x200000)) {
        u32 *state = &a->chase.base.statusFlags;
        s32 mask = ~0x1000000;
        *state = mask & (*state);
        {
            s32 mask = ~0x200;
            *flags = mask & (*flags);
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        s32 mask = ~0x400;
        *flags = mask & (*flags);
    }
    a->chase.group.turn.turnsRemaining = 255;
}

void Vorn_BeginResponsePursuit(struct Vorn *a, u32 reset, const void *unused, u32 unused2)
{
    struct Vorn *ctx = (void *)a->chase.base.context;
    struct SceneActorChaseGroup *group = &ctx->chase.group;
    struct VornResponseFlags *flags;

    if (ctx->field69E)
        return;
    if (Vorn_TestContext(ctx, 1, &flags) && !Vorn_TestResponseFlags(flags, 2))
        return;
    ctx->field69E = 60;
    Vorn_ResetContext(ctx);
    Vorn_SetResponseFlags(flags, 1);
    flags->bits &= ~2;
    ctx->field698 = ctx->frame69A;
    a->chase.base.body.direction = a->chase.base.reactionDirection;
    if (reset == 1)
        SceneBody_StartRoute(&a->chase.base.body.id, &group->homeRoute,
                             group->homeRoute.branches[1 ^ group->homeRoute.activeBranch], 0, 0);
    Vorn_SelectRouteAwayFromTarget(&a->chase);
    {
        u32 mask = 4;
        a->chase.base.statusFlags = mask | a->chase.base.statusFlags;
    }
    {
        s8 mask = ~16;
        a->chase.base.behaviorFlags = mask & a->chase.base.behaviorFlags;
    }
    SceneActorChase_SetBehavior(a, 15);
    SceneActorChase_SetControl(a, a->chase.base.onStanding, 0);
    Vorn_SetTimedMotion(a, Vorn_UpdatePursuitWait, 35);
    Vorn_SetTrackingFlags(&a->chase.base.tracking, 8);
    {
        u32 mask = 128;
        a->chase.base.behaviorFlags = mask | a->chase.base.behaviorFlags;
    }
}

static inline u32 TestMotionFlag(struct Vorn *p, u32 mask)
{
    return (p->chase.base.motionFlags & mask);
}

u8 SceneActor_IsFacingNearbyTarget(struct Vorn *p)
{
    struct Vorn *q = p->chase.base.context;
    s32 absolute;
    if (q->field6E8)
        return 0;
    if (TestMotionFlag(p, 0x100))
        return 0;
    if (p->chase.base.tracking.distance > 0xFFFF)
        return 0;
    absolute =
        RotationPhase_HeadingDistance(p->chase.base.tracking.heading, p->chase.base.body.direction);
    if (absolute > 0x12)
        return 0;
    return 1;
}

struct QUpdateRecoveryCooldown {
    u8 f[0x6e8];
    u16 v;
};

struct PUpdateRecoveryCooldown {
    u8 f[0x3d0];
    struct QUpdateRecoveryCooldown *q;
};

void Vorn_UpdateRecoveryCooldown(struct PUpdateRecoveryCooldown *p)
{
    u16 *v = &p->q->v;
    if (*v != 0)
        *v = *v - 1;
}

void Vorn_ResetRecoveryMode(u8 *p)
{
    p[0x6c0] = 0;
}

void SceneStimulus_AddTagged(s16 a, s16 b, s16 c, s16 d, u8 e, u16 f);

void Vorn_TickSound(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (ctx->field6EC == 0) {
        struct SceneVec *vec;

        ctx->field6EC = 60;
        Sound_PlaySong(0x1CF);
        vec = Player_GetEffectivePosition(0);
        if (vec != 0) {
            SceneStimulus_AddTagged(vec->x, vec->z, 0, 0, 0x80, (u16)actor->base.body.id);
        }
    } else {
        ctx->field6EC--;
    }
}

static inline u32 Vorn_CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 Vorn_CellHeight(struct ContactPosition *position, u32 cell)
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
    height -= (u8)position->z;
    goto done;
slopeX:
    height -= (u8)position->x;
done:
    return height;
}

static inline void Vorn_ClearRouteBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline void Vorn_SetControl(struct Vorn *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->chase.base.field2B2 = 1;
    actor->chase.base.stepDone = 0;
    actor->chase.base.controlKind = kind;
    actor->chase.base.target = callback;
    actor->chase.base.field2C8 = 0;
    {
        u32 *field = &actor->chase.base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->chase.base.pendingControl = 0;
}

/* Restores the saved route after a context response; interpretation is tentative. */

void Vorn_TickSound(struct ChaseActor *actor);

void Vorn_SendMessage(struct ChaseActor *actor);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline u32 Vorn_Mask16(struct Vorn *p, u32 mask)
{
    return p->chase.base.motionFlags & mask;
}

static inline u32 Vorn_Mask32(struct Vorn *p, u32 mask)
{
    return p->chase.base.statusFlags & mask;
}

static inline u8 Vorn_IsInactive(struct Vorn *p)
{
    if (p && (Vorn_Mask16(p, 0x40) || Vorn_Mask32(p, 0x20000) || p->chase.base.health <= 0))
        return 1;
    return 0;
}

static inline u8 Vorn_TestBody(void *p, u32 mask)
{
    if (((struct Vorn *)p)->flags6D4 & mask)
        return 1;
    return 0;
}

static inline u32 Vorn_MaskLow(struct Vorn *p, u32 mask)
{
    return p->chase.base.behaviorFlags & mask;
}

static inline void Vorn_SnapTarget(struct ContactPosition *out,
                                   struct SceneActorChaseRoutePoint *point)
{
    out->x = point->x;
    out->z = point->z;
    out->y = Vorn_CellHeight(out, Vorn_CellIndex(out));
}

static inline void Vorn_Resume(struct Vorn *input, struct SceneActorChaseGroup *group, u32 *flags)
{
    struct Vorn *actor = input;
    struct ContactPosition target;
    struct SceneActorRegistryRoute *point;
    actor->chase.base.field2C2 = 64;
    actor->chase.group.common.gate.flags = 0;
    actor->chase.group.common.gate.counter = 0;
    Vorn_ClearRouteBits(&actor->chase.base.motionFlags, 0x380);
    {
        struct SceneActorChaseTracking *motion = &actor->chase.base.tracking;
        u32 mask = ~0x18;
        motion->flags = mask & motion->flags;
    }
    {
        struct SceneActorRegistryControl *control = &actor->chase.group.common.control;
        if (Vorn_MaskLow(actor, 0x200000)) {
            SceneActorChase_ClearWordFlags(flags, 0x1000000);
            Vorn_ClearRouteBits(&actor->chase.base.motionFlags, 0x200);
            {
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
        }
    }
    Vorn_ClearRouteBits(&actor->chase.base.motionFlags, 0x400);
    actor->chase.group.turn.turnsRemaining = 255;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, 0x19);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    Vorn_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    {
        u8 k1 = 3;
        actor->chase.base.state2A8 = k1;
    }
    actor->chase.base.phase2A9 = 1;
    actor->chase.base.behavior2AA = 0;
    actor->chase.base.counter2AC = 0;
    actor->chase.base.field2B1 = 1;
    {
        struct SceneActorDelayedResponse *command = (void *)&group->saved;
        u32 target = (u32)actor->chase.base.onAlternateEntry;
        u32 kind = 7;
        command->delay = 60;
        command->control = (void *)target;
        command->state = 0;
        command->phase = 0;
        command->behavior = kind;
    }
    {
        u32 mask = 16;
        u32 *field = &actor->chase.base.behaviorFlags;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    Vorn_ClearRouteBits(&actor->chase.base.reactionFlags, 0x9c9);
    SceneBody_StartRoute(&actor->chase.base.body.id, &group->homeRoute, group->homeRoute.routeId,
                         group->homeRoute.parameterB, group->homeRoute.pointId);
    point = &group->homeRoute;
    Vorn_SnapTarget(&target, point->point);
    group->destination->pad = 1;
    if (!SceneActorChase_TryRoute(&actor->chase, group->route, &input->chase.base.body.position,
                                  &target))
        Callback_InvokeActor(input, group->fallback);
}

void Vorn_ResumeRouteAfterReaction(void *object)
{
    struct Vorn *actor = object;
    struct SceneActorChaseGroup *group = &actor->chase.group;
    struct Vorn *body;
    u32 *flags;
    u32 gateCounter;
    if (Vorn_IsInactive(actor))
        return;
    if (Vorn_Mask16(actor, 0xe33f))
        return;
    if (!Vorn_Mask16(actor, 0x80))
        return;
    {
        u32 mask = 4;
        u32 *field = &actor->chase.base.statusFlags;
        u32 value = *field;
        value |= mask;
        *field = value;
    }
    body = actor->chase.base.context;
    gateCounter = group->common.gate.counter;
    flags = &actor->chase.base.statusFlags;
    if (gateCounter) {
        if ((actor->chase.base.controlKind == 0x25) && Vorn_TestBody(body, 4))
            Vorn_TickSound(object);
        return;
    }
    if (Vorn_TestBody(body, 4)) {
        Vorn_Resume(actor, group, flags);
        Vorn_ClearRouteBits(&((struct Vorn *)actor->chase.base.context)->flags6D4, 6);
        SceneActorChase_ClearWordFlags(flags, 4);
        Vorn_SendMessage(&actor->chase);
    } else if (Vorn_TestBody(body, 16)) {
        Vorn_Resume(actor, group, flags);
        Vorn_ClearRouteBits(&((struct Vorn *)actor->chase.base.context)->flags6D4, 0x70);
        SceneActorChase_ClearWordFlags(flags, 4);
        Vorn_SendMessage(&actor->chase);
    }
}

// confidence=low: dispatches response, pursuit, and retreat actions from context flags.

void Vorn_BeginResponsePursuit(struct Vorn *, u32, const void *, u32);

u8 Vorn_BeginPursuitRoute(struct Vorn *actor, u32 reset);

u32 Vorn_SelectRetreatRoute(struct ChaseActor *actor, u32 reset);

void Vorn_SelectRecoveryAction(u8 *, const u8 *, u32);

u8 SceneActor_IsFacingNearbyTarget(struct Vorn *);

static inline u8 TestActor(struct Vorn *p, u32 mask)
{
    if (p->flags6D4 & mask)
        return 1;
    return 0;
}

static inline void SetFlags(u16 bit, u16 *field)
{
    struct Flags {
        s16 bits : 16;
    };
    struct Flags *p = (void *)field;
    p->bits |= bit;
}

static inline u8 TestValue(u32 value)
{
    if (value)
        return 1;
    return 0;
}

u8 SceneActor_DispatchContextAction(struct Vorn *q, u32 reset)
{
    struct Vorn *p = q;
    u16 *flags;
    u32 first;
    if (TestActor(p, 1)) {
        if (reset == 1) {
            q->field69E = 0;
            SetFlags(2, &q->flags6D4);
        }
        Vorn_BeginResponsePursuit(p, reset, gUnk_0824E230, 0x60B);
        return 1;
    }
    {
        u32 mask = 4;
        u16 *ptr = &q->flags6D4;
        first = *ptr & mask;
        flags = ptr;
        first = TestValue(first);
    }
    if (first) {
        if (reset == 1)
            q->pursuitCooldown = 0;
        Vorn_BeginPursuitRoute(p, reset);
        return 1;
    }
    if (Test(flags, 0x10)) {
        if (reset == 1)
            q->field6D0 = 0;
        if (SceneActor_IsFacingNearbyTarget(p)) {
            Vorn_SelectRecoveryAction(p, gUnk_0824E230, 0x61F);
            return 1;
        }
        Vorn_SelectRetreatRoute(&p->chase, reset);
        return 1;
    }
    return 0;
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

// confidence=low: enters directional animation three when the contact-ready flag is set.

u8 Vorn_BeginContactAnimation(struct ChaseActor *actor)
{
    if (actor->group.hitReactionLatch == 0 && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 45;
        actor->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    return SceneActor_DispatchContextAction(actor, 0);
}

void SceneActorTarget_ResetContactResponse(void *);

u8 Vorn_UpdateContactResponse(struct ChaseActor *actor)
{
    struct SceneActorContactTracking *motion;
    u16 zero;
    u16 *motionFlags;
    void *work;
    void **nodeSlot;
    struct SceneActorContactNode *contact;
    struct SceneActorContactTarget *target;
    u16 cell;
    u32 active;
    u32 *actionFlags;
    work = actor;
    if (SceneActorContact_MotionFlags(SceneActorContact_ActorFromWork(&work), 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.targetQuery))
        return 1;
    if (!SceneActorContact_TestTrackingOwned((void *)&actor->base.tracking, 1, &motion))
        return 0;
    motionFlags = SceneActorContact_MotionField(actor);
    {
        u16 flags;
        SCENE_ACTOR_CONTACT_READ_MOTION_FIELD(flags, motionFlags);
        zero = flags & 0x100;
    }
    if (zero)
        return 0;
    nodeSlot = &actor->group.contactNode;
    if (!actor->base.tracking.limit)
        return 0;
    SCENE_ACTOR_CONTACT_FIND_CONTACT(contact, actor);
    if (!contact)
        return 0;
    if (SceneActorContact_QueryMotionField(actor, 0x1000, &work)) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        SceneActorChase_ClearBits(work, 0x1000);
        *nodeSlot = (void *)zero;
    }
    target = contact->object;
    motion->tracking.target = &target->target;
    cell = target->cell->index;
    work = actor;
    SceneActorChase_ClearFlagComponent(SceneActorContact_GateFromWork(&work), 2);
    if (motion->active) {
        if (SceneActorContact_TestTracking(motion, 8)) {
            if (SceneActorContact_SavedTargetCell(actor) != cell)
                SceneActorContact_SaveTargetCell(actor, cell);
        } else
            SceneActorTarget_ResetContactResponse(actor);
        SceneActorChase_SetReactionFlags(actor, 0x80);
        SceneActorContact_SetTrackingFlags(motion, 0x10);
        return 0;
    }
    active = SceneActorContact_ReadActorFlags(actor, 0x80, &actionFlags);
    if (active)
        return 0;
    *actionFlags |= 0x80;
    SceneActorChase_ClearBits(motionFlags, 0x100);
    SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E230, 0x689);
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_ClearBits(motionFlags, 0x400);
    actor->group.turn.turnsRemaining = -1;
    SceneActorAnimation_SelectDirectionalEntry(SceneActorContact_ActorFromWork(&work), 0);
    actor->group.common.control.timer = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
    SceneActorChase_SetReactionFlags(actor, 0x100);
    return 1;
}

u8 Vorn_BeginContactAnimation(struct ChaseActor *);

static inline u8 Test_SelectHitRecovery(struct Vorn *context, u32 mask)
{
    if (context->flags6D4 & mask)
        return 1;
    return 0;
}

// confidence=low: pending hit flags select recovery control or pursuit/retreat behavior.

u8 Vorn_SelectHitRecovery(struct ChaseActor *actor)
{
    struct Vorn *context = actor->base.context;
    u32 blocked;
    {
        u16 *flags = &context->flags6D4;
        s32 mask = ~8;
        *flags = *flags & mask;
    }
    if (!(actor->base.hitAttributes & 0x20000)) {
        if (actor->base.reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x1CB);
    }
    {
        u32 mask = 0x8002;
        blocked = mask & actor->base.motionFlags;
    }
    if (blocked) {
        {
            u32 mask = ~0x20;
            mask &= actor->base.motionFlags;
            actor->base.motionFlags = mask;
        }
        return 0;
    }
    if (actor->base.hitReactionFlags & 0xC0000) {
        void *control = actor->base.onReaction;
        u32 kind = 0x12;
        u8 zero8;
        actor->base.field2B2 = 1;
        actor->base.stepDone = blocked;
        actor->base.controlKind = kind;
        actor->base.target = control;
        zero8 = 0;
        actor->base.field2C8 = blocked;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->base.pendingControl = blocked;
        {
            u32 state = 3;
            actor->base.state2A8 = state;
        }
        actor->base.phase2A9 = zero8;
        actor->base.behavior2AA = zero8;
        actor->base.counter2AC = blocked;
        actor->base.field2B1 = 1;
        {
            u32 *flags = &actor->base.statusFlags;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        return 1;
    }
    {
        u32 mask = ~0x20;
        actor->base.motionFlags &= mask;
    }
    if (actor->base.motionFlags & 0x100)
        return 0;
    if (actor->base.hitLockTimer)
        return 0;
    if (Test_SelectHitRecovery(context, 0x1D))
        return Vorn_BeginContactAnimation((void *)context);
    if (actor->group.hitReactionLatch == 0 && (actor->base.field2E6 & 8)) {
        actor->base.hitLockTimer = 45;
        actor->group.hitReactionLatch = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    if (!actor->base.tracking.target)
        return 0;
    {
        u32 foreign = context->foreignPosition;
        if (foreign == 0) {
            context->pursuitCooldown = foreign;
            Vorn_BeginPursuitRoute(actor, 1);
        } else {
            Vorn_SelectRetreatRoute(actor, 1);
        }
        return 1;
    }
}

void Vorn_ResumeRouteAfterReaction(void *);

void SceneActor_TickRandomReaction(struct ChaseActor *p);

void Vorn_TryUphillRoute(struct ChaseActor *actor);

void SceneActorPhase_TryBeginSpecialMotion(struct ChaseActor *p);

void SceneActor_TrySoloAction(struct Vorn *p);

s32 Vorn_UpdateBehavior(void *p)
{
    Vorn_ResumeRouteAfterReaction(p);
    SceneActor_TickRandomReaction(p);
    Vorn_TryUphillRoute(p);
    SceneActorPhase_TryBeginSpecialMotion(p);
    SceneActor_TrySoloAction(p);
    return 1;
}

void Vorn_UpdateRecoveryCooldown(struct PUpdateRecoveryCooldown *p);

u32 Vorn_UpdateActionTimers(u8 *p)
{
    u8 *q = p;
    u16 *r;
    Vorn_UpdateRecoveryCooldown(p);
    switch (p[0x2B5]) {
        case 12:
        case 13:
        case 34:
        case 35:
        case 37:
        case 41:
        case 42: {
            u32 m = 4;
            u32 *x = (u32 *)(q + 0x2D0);
            *x = *x | m;
        } break;
    }
    r = (u16 *)(p + 0x69E);
    if (*r != 0)
        *r = *r - 1;
    r = (u16 *)(p + 0x6AC);
    if (*r != 0)
        *r = *r - 1;
    r = (u16 *)(p + 0x6D0);
    if (*r != 0)
        *r = *r - 1;
    return 1;
}

void SceneActorRegistry_SendToCategory(s32, s16 *);

void Vorn_SendMessage(struct ChaseActor *actor)
{
    struct Vorn *ctx = (struct Vorn *)actor->base.context;

    if (ctx->field6C1 != 0) {
        s16 message[2];

        message[0] = 7;
        message[1] = actor->base.body.id;
        SceneActorRegistry_SendToCategory(0x19, message);
        ctx->field6C1 = 0;
    }
}

struct QBroadcastPosition {
    u8 f[0x6c1];
    u8 v;
};

struct PBroadcastPosition {
    u8 f[0x50];
    s32 x;
    u8 g[0x3d0 - 0x54];
    struct QBroadcastPosition *q;
};

void Vorn_BroadcastPosition(struct PBroadcastPosition *p)
{
    struct QBroadcastPosition *q = p->q;
    s16 s[2];
    s[0] = 6;
    s[1] = p->x;
    SceneActorRegistry_SendToCategory(0x19, s);
    q->v = 1;
}

struct VornMessageActor {
    u8 unknown0[0x50];
    u32 identity;
    u8 unknown54[0x37C];
    u8 *context;
};

// confidence=low: category messages six/seven toggle the foreign-position notification latch.

void Vorn_LatchForeignPosition(struct RoomActor *actor, const u16 *message)
{
    if (!SceneActorPlacement_IsUnavailable(actor)) {
        struct VornMessageActor *view = (void *)actor;
        if (message[1] != view->identity)
            view->context[0x6C0] = 1;
    }
}

// confidence=low: category messages six/seven toggle the foreign-position notification latch.

void Vorn_ClearForeignPosition(struct RoomActor *actor, const u16 *message)
{
    if (!SceneActorPlacement_IsUnavailable(actor)) {
        struct VornMessageActor *view = (void *)actor;
        if (message[1] != view->identity)
            view->context[0x6C0] = 0;
    }
}

u32 Trigger_ConsumePending(void *receiver, u32 count);

struct SRestoreControlAfterItemLaunch {
    u8 pad[0x9C];
    s16 z;
    u8 pad2[0xDC - 0x9E];
    u8 extra[0x10];
    u8 pad3[0x104 - 0xEC];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6a[0x22C - 0x220];
    void (*next)(void);
    u8 pad6[0x2A8 - 0x230];
    u8 a;
    u8 b;
    u8 c;
    u8 pad7;
    u32 count;
    u8 done;
    u8 queued;
    u8 taken;
    u8 pad8[2];
    u8 code;
    u8 pad9[0x2C8 - 0x2B6];
    u16 timer;
    u8 pad10[0x2D0 - 0x2CA];
    u32 flags;
    u16 hflags;
    u8 pad11[0x3D0 - 0x2D6];
    struct SRestoreControlAfterItemLaunch *other;
    u32 at3D4;
};

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

s32 SceneActor_TryLaunchItem13(struct SceneActorItemLaunch *p);

// confidence=low: launches the context item, consumes the trigger and restores saved control.

void Vorn_RestoreControlAfterItemLaunch(struct SRestoreControlAfterItemLaunch *s)
{
    struct SRestoreControlAfterItemLaunch *context = s->other;
    if (TakeFlag(&s->queued)) {
        void (*fn)(void);
        u32 zero;
        u8 one;
        u8 zero8;
        u32 *flags;
        u32 two;
        SceneActor_TryLaunchItem13(context);
        ((struct Vorn *)context)->response = 0xFFFF;
        Trigger_ConsumePending(((u8 *)s) + 0xC, 1);
        fn = s->next;
        zero = 0;
        {
            u8 *taken = &s->taken;
            one = 1;
            *taken = one;
        }
        s->done = zero;
        s->code = zero;
        s->callback = fn;
        zero8 = 0;
        s->timer = zero;
        flags = &s->flags114;
        {
            s32 mask = -2;
            *flags &= mask;
        }
        s->at104 = zero;
        two = 2;
        s->a = two;
        s->b = zero8;
        s->c = two;
        s->count = zero;
        s->queued = one;
    } else {
        ++s->count;
    }
}

extern const u32 gUnk_08606010[];

void Vorn_InitActionTable(u8 *p)
{
    *(const u32 **)(p + 0x29c) = gUnk_08606010;
}

s32 Vorn_UpdateBehavior(void *);

u32 Vorn_UpdateActionTimers(u8 *);

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

struct SOnUpdate {
    u8 filler1[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Vorn_OnUpdate(struct SOnUpdate *s)
{
    u32 m;
    Vorn_UpdateBehavior(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        {
            u8 *c = (u8 *)s + 0xDC;
            u32 mask = ~0x20;
            c[0xF] &= mask;
        }
        SceneActorControl_DispatchControl(s);
    }
    Vorn_UpdateActionTimers((u8 *)s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

u32 Vorn_OnRender(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void);

u32 Vorn_OnDestroy(u8 *unused)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}

void *Heap_Alloc(s32 size);

void Memory_Clear(void *dst, s32 bytes);

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_SetMotionMode(void);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

u32 Route_GetCurrentPointFacing(void *);

s32 TargetRegistry_RegisterActor(void *, void *, void *);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);

u8 Vorn_UpdateContactResponse(struct ChaseActor *actor);

void SceneActorPlacement_ResetStateThree(void);

extern const u8 gUnk_08606038[];

static inline void VornInit_BeginBehavior(struct ChaseActor *actor, u32 behavior)
{
    MonsterInit_SetState(actor, 0, 0, behavior);
}

static inline void VornInit_ResetFlags(void *object)
{
    struct Vorn *actor = object;
    actor->flags6D4 = 0;
}

static inline void VornInit_InitializeBody(void *object, u32 *fill)
{
    struct ChaseActor *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = MonsterInit_ReadKeywordHalfword('M', 0);
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
    arg0->base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

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
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->base.hudLabel = identity;
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

static inline void VornInit_ReadPalette(struct ChaseActor *actor, u32 base, u32 alternate)
{
    s32 palette;
    actor->base.paletteOffset = Script_GetKeywordValue(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    actor->base.alternateOffset = MonsterInit_GetPaletteOffset(actor) + base + 1;
    palette = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(actor->base.drawable + 0x2C, palette);
    else
        *(u8 **)(actor->base.drawable + 0x68) = gPaletteBankData + (palette << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetScriptPalette((void *)(actor->base.drawable + 0x20),
                                           SceneActorScript_GetManagerField62());
}

void Vorn_InitActorProfile(struct PInitActorProfile *p);

void SceneActor_InitContextColliders(struct ChaseActor *p);

s32 Vorn_OnUpdate(struct SOnUpdate *s);

u16 Vorn_GetAnimationEvent(u8 *p);

void Vorn_UpdateIdleFacing(void *);

void Vorn_ResetStateEntry(void *);

void Vorn_BeginIdle(void *);

void Vorn_UpdateStaggerRecovery(void *);

void Vorn_UpdateDefeat(void *);

void Vorn_UpdateHiddenRecovery(void *);

void SceneActorTarget_ResolvePendingObject(void);

void Vorn_OnReactionNoOp(void);

void SceneActorReaction_ResumeSavedTarget(void);

void SceneActor_ResetContextAnimation(struct Vorn *p);

void Vorn_SelectReactionControl(struct ChaseActor *a);

u8 Vorn_SelectHitRecovery(struct ChaseActor *actor);

void Vorn_OnAnimationCleanup(void);

void Vorn_BeginTargetPursuit(struct Vorn *actor);

u32 Vorn_OnRender(void);

u32 Vorn_OnDestroy(u8 *);

static inline void VornInit_SetReactionTable(struct ChaseActor *actor, const void *table)
{
    MonsterInit_WriteDataPointer(&actor->base.commandTable.opaque, table);
}

static inline void VornInit_EnableInitialFlags(struct ChaseActor *actor)
{
    MonsterInit_SetTargetingFlags(&actor->base.tracking, 1);
    MonsterInit_SetBehaviorFlags(actor, 0x1600000);
    MonsterInit_SetStatusFlags(actor, 0x8000000);
}

static inline void VornInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Vorn_OnRender;
    arg0->base.destroy = (SceneActorResultCallback)Vorn_OnDestroy;
}

void Vorn_ParseField624(struct ChaseActor *actor);

void Vorn_InitScriptCallback(struct Vorn *p);

void Vorn_InitActionCooldown(u8 *p);

void Vorn_InitTimedAction(struct P *p);

void Vorn_ParseField69A(struct Vorn *ctx);

void Vorn_ResetRecoveryMode(u8 *p);

void Vorn_InitMotionLimits(struct PInitMotionLimits *p);

void Vorn_ParseField688(struct Vorn *ctx);

void Vorn_InitAttackActionTable(u8 *p);

void Vorn_InitSpecialActionTable(u8 *p);

void Vorn_InitReactionActionTable(u8 *p);

void Vorn_InitActionTable(u8 *p);

void ActorShadow_Disable(void *);

void SceneActorReaction_BeginSideStep(void);

s32 Vorn_TryRestorePatrolRoute(void *);

void Vorn_UpdateKnockbackRecovery(void);

void SceneActorReaction_ResumeOrRunScript(void);

void Vorn_ResumeReactionRoute(void);

s32 Vorn_Init(void *object)
{
    struct Vorn *vorn = object;
    struct ChaseActor *actor = object;
    struct SceneActorChaseGroup *group = &vorn->chase.group;
    struct SceneActorBody *body = &vorn->chase.base.body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 mode;

    VornInit_ResetFlags(vorn);
    SceneActorRegistry_SetObjectContext(actor, actor, 1, 1);
    MonsterInit_SetCategory(actor, 0x19);
    Vorn_InitActorProfile(actor);
    Vorn_ParseField624(actor);
    Vorn_InitScriptCallback(actor);
    Vorn_InitActionCooldown(actor);
    Vorn_InitTimedAction(actor);
    Vorn_ParseField69A(actor);
    Vorn_ResetRecoveryMode(actor);
    Vorn_InitMotionLimits(actor);
    Vorn_ParseField688(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x3DB6, 0);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x3DB6, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    VornInit_InitializeBody(vorn, &zero);
    VornInit_ReadPalette(actor, 0x31B, 0x192);
    SceneActor_InitContextColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        actor, Vorn_OnUpdate, Vorn_GetAnimationEvent, Vorn_UpdateIdleFacing, Vorn_ResetStateEntry,
        Vorn_BeginIdle, Vorn_UpdateHiddenRecovery, Vorn_UpdateStaggerRecovery, Vorn_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, Vorn_UpdateContactResponse,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(group, &vorn->routeProbe, &vorn->destination,
                                          Vorn_OnReactionNoOp, SceneActorReaction_ResumeSavedTarget,
                                          Vorn_TryRestorePatrolRoute, 0, 1, body);
    SceneActorMovement_ResolveRoutePosition(vorn);
    MonsterInit_SetRouteReset(actor, SceneActor_ResetContextAnimation);
    MonsterInit_SetReactionGate(actor, Vorn_SelectReactionControl);
    MonsterInit_SetContactCallback(actor, Vorn_SelectHitRecovery);
    VornInit_SetReactionTable(actor, gUnk_08606038);
    group->contactControl = Vorn_UpdateKnockbackRecovery;
    group->field5E0 = SceneActorReaction_ResumeOrRunScript;
    group->responseCallback = Vorn_OnAnimationCleanup;
    group->field5EC = Vorn_BeginTargetPursuit;
    group->field614 = Vorn_ResumeReactionRoute;
    actor->base.field2E8 = 3;
    VornInit_EnableInitialFlags(actor);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(actor);
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
    SceneActorRegistry_RegisterCategoryReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Vorn_InitAttackActionTable(actor);
    Vorn_InitSpecialActionTable(actor);
    Vorn_InitReactionActionTable(actor);
    Vorn_InitActionTable(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);

    flag = vorn->field68C;
    if (flag == 1) {
        if (MonsterInit_GetDrawableKind(actor) == 0)
            *(u32 *)actor->base.drawable |= flag;
        else {
            u8 *widget = actor->base.drawable + 0x20;
            *(u32 *)(widget + 8) |= flag;
        }
        ActorShadow_Disable(actor->base.shadow);
        MonsterInit_SetState(actor, 3, 0, 0);
        MonsterInit_SetControl(actor, actor->base.onHidden, 0xC);
    } else if ((mode = Script_GetKeywordValue(0x54, 0)) != 0) {
        if (mode == 1)
            vorn->axis = 0;
        else
            vorn->axis = 1;
        VornInit_BeginBehavior(actor, 0x12);
        MonsterInit_SetControl(actor, actor->base.onStanding, 0);
        MonsterInit_SetMotionControl(actor, SceneActorReaction_BeginSideStep, 0x26);
    }
    VornInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Vorn_Init(void *object);

void Heap_Free(void *);

void Vorn_Create(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (Vorn_Init(p) < 0) {
            Vorn_OnDestroy(p);
            Heap_Free(p);
        }
    }
}

static inline u8 Test_HasActiveResponse(struct Vorn *actor, u32 mask)
{
    if (actor->flags6D4 & mask)
        return 1;
    return 0;
}

// confidence=low: reports whether a response flag or nonnegative response distance is present.

u8 Vorn_HasActiveResponse(struct Vorn *context)
{
    if (!Test_HasActiveResponse(context, 1) && context->response < 0)
        return 0;
    return 1;
}
