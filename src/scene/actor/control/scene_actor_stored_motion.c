#include "global.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "engine/engine.h"
#include "player/player.h"
#include "engine/ram.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_visibility.h"
#include "scene/actor/scene_actor_state.h"
#include "scene/actor/scene_actor_placement.h"
#include "scene/actor/scene_actor_status.h"

/* SceneActorControl_ReturnSuccess: Returns constant 1; provisional success stub with no known callers */

s32 SceneActorControl_ReturnSuccess(void)
{
    return 1;
}

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

s32 Callback_InvokeActor(void *, void *);

// concession-ok(literal-param): one truncating shift used with widths 3, 4 and 12; the width written in place gives the same code
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline u32 WanderRand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + (u32)gRandomTable);
}

/* The block at 0x3F0 of the control context; the position curve writes its position at 0x3FC. */
struct StoredCurve {
    u8 unknown0[0xC];
    struct ContactPosition position;
};

static inline u8 ApproachStoredPoint(struct ChaseActor *actor, struct StoredCurve *curve,
                                     s32 inSpeed)
{
    struct SceneActorBody *motion = &actor->base.body;
    s32 speed = inSpeed;
    s16 dx = curve->position.x - (u16)motion->position.x;
    s16 dz = curve->position.z - (u16)motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x += Shift(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        motion->motion.z += Shift(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

static inline u8 MotionState_TestFlags(struct SceneActorControlMotionState *state, u32 mask)
{
    if (state->flags3D4 & mask)
        return 1;
    return 0;
}

static inline void MotionState_SetFlags(struct SceneActorControlMotionState *state, u32 mask)
{
    state->flags3D4 |= mask;
}

static inline void MotionState_ClearFlags(struct SceneActorControlMotionState *state, u32 mask)
{
    state->flags3D4 &= ~mask;
}

// confidence=low: homes on a stored point, then picks a new point around the player.

void SceneActorControl_WanderAroundPlayer(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        struct SceneActorControlMotionState *context = actor->base.context;
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&context->vectors[2]);
        context->swayStep = 1;
    }
    SceneActorControl_TickSway(actor);
    if (ApproachStoredPoint(actor, (struct StoredCurve *)body->unknown3F0,
                            Callback_InvokeActor(actor, actor->base.animationQuery.opaque))) {
        if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
            struct ContactPosition target;
            u32 angle;
            angle = (u8)WanderRand();
            target = *(struct ContactPosition *)&gPlayers[0]->position;
            target.x += Shift(gUnk_086149C4[angle], 4);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 4);
            SceneActorControl_StartPositionCurve(actor, &target);
            MotionState_ClearFlags(actor->base.context, 8);
        }
    } else {
        actor->base.counter2AC++;
    }
}

void SceneActorControl_UpdateTimedFacing(struct ChaseActor *);

void SceneActorControl_ResetCollisionPhase(struct ChaseActor *);

static inline u32 MotionRand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + RANDOM_TABLE_ADDR);
}

// confidence=low: waits a timed interval, homes on a stored point, then picks a new point.

void SceneActorControl_UpdateStoredMotion(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorTerrain_InstallControl(actor, (void *)SceneActorControl_UpdateTimedFacing, 0x24);
        ZeroFill8(&body->vectors[2]);
        body->swayStep = 1;
        body->waitTimer = 0;
        {
            u32 angle;
            struct ContactPosition target;
            angle = (u8)MotionRand();
            target = *(struct ContactPosition *)&gPlayers[0]->position;
            target.x += Shift(gUnk_086149C4[angle], 3);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
            SceneActorControl_InitPositionCurveEndpoints(actor, &target);
            SceneActorControl_AdvancePositionCurve(actor);
            MotionState_ClearFlags(body, 8);
        }
    }
    if (body->waitTimer != 0) {
        u32 timer = body->waitTimer;
        timer--;
        body->waitTimer = timer;
        if ((timer << 16) == 0) {
            SceneActorControl_StartPositionCurve(
                actor, (const struct ContactPosition *)&gPlayers[0]->position);
            MotionState_SetFlags(body, 8);
        }
    } else {
        SceneActorControl_TickSway(actor);
        if (ApproachStoredPoint(actor, (struct StoredCurve *)body->unknown3F0,
                                Callback_InvokeActor(actor, actor->base.animationQuery.opaque))) {
            if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
                struct ContactPosition target;
                u32 angle;
                angle = (u8)MotionRand();
                target = *(struct ContactPosition *)&gPlayers[0]->position;
                target.x += Shift(gUnk_086149C4[angle], 3);
                target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
                SceneActorControl_InitPositionCurveEndpoints(actor, &target);
                SceneActorControl_AdvancePositionCurve(actor);
            }
        }
        if (actor->base.tracking.distance > 0xFDE8) {
            SceneActorControl_ResetCollisionPhase(actor);
            actor->base.body.direction = actor->base.tracking.heading;
            body->waitTimer = 0x1E;
            return;
        }
    }
    if (actor->base.stepDone) {
        SceneActorTerrain_EnterState(actor, 0, 0, 4);
    } else {
        actor->base.counter2AC++;
    }
}

extern u8 gUnk_08605C5C[];

void SceneActorControl_SetApproachCallbacks(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605C5C;
}

static inline void SetDrawableAndShadow(struct ChaseActor *actor, u32 visible)
{
    if (visible) {
        ActorStatus_ClearDrawableFlags(actor, 1);
        ActorShadow_Enable(actor->base.shadow);
    } else {
        ActorStatus_SetDrawableFlags(actor, 1);
        ActorShadow_Disable(actor->base.shadow);
    }
}
void SceneActorControl_TickVisibility(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state;
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (unavailable)
        return;
    state = SceneActorControl_GetMotionState(actor);
    if (!MotionState_TestFlags(state, 0x60))
        return;
    {
        u32 counter;
        u32 value;
        {
            u16 *field = &state->visibilityFrame;
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            SetDrawableAndShadow(actor, (u8)(counter >> 18) & 1);
        } else if (value <= 29) {
            SetDrawableAndShadow(actor, (u8)(counter >> 17) & 1);
        } else if (MotionState_TestFlags(state, 0x20)) {
            ActorStatus_ClearDrawableFlags(actor, 1);
            MotionState_ClearFlags(state, 0x20);
            ActorShadow_Enable(actor->base.shadow);
        } else {
            ActorStatus_SetDrawableFlags(actor, 1);
            MotionState_ClearFlags(state, 0x40);
            ActorShadow_Disable(actor->base.shadow);
        }
    }
    {
        u16 *counter = &state->visibilityFrame;
        (*counter)++;
    }
}

void SceneActorControl_StartStoredCurve(struct ChaseActor *actor)
{
    struct ContactPosition position;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    position = actor->base.previousPosition;
    SceneActorControl_StartPositionCurve(actor, &position);
    SceneActorTerrain_EnterState(actor, 2, 0, 3);
    MotionState_ClearFlags(SceneActorControl_GetMotionState(actor), 8);
    actor->base.field2B1 = 0;
}

static inline u32 ReadFlags16(struct ChaseActor *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u16 *flags;
    flags = &actor->base.motionFlags;
    return *flags & mask;
}

static inline u32 ReadFlags32(struct ChaseActor *actor, u32 inputMask)
{
    u32 mask = inputMask;
    u32 *flags;
    flags = &actor->base.statusFlags;
    return *flags & mask;
}

static inline u8 IsUnavailable(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    if (actor) {
        if (ReadFlags16(actor, 0x40) || ReadFlags32(actor, 0x20000) || actor->base.health <= 0)
            return 1;
    }
    return 0;
}

/* actor must be nonnull: its control context is loaded before the availability gate. */

void SceneActorControl_RequestVisibility(struct ChaseActor *input, const u16 *inputRequest)
{
    struct ChaseActor *actor = input;
    const u16 *request = inputRequest;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    u32 unavailable = IsUnavailable(actor);
    if (unavailable)
        return;
    if (!SceneActor_MaskedHalfword(0x4000, &actor->base.motionFlags))
        return;
    SceneActorPlacement_SetFlags(0x800, &actor->base);
    {
        u32 requested = request[1];
        if (requested == 0) {
            u32 mask = 0x20;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = &state->flags3D4;
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (hidden) {
                    state->visibilityFrame = 0;
                    {
                        u32 mask = ~0x50;
                        {
                            u32 old = *savedFlags & mask;
                            *savedFlags = old | 0x20;
                        }
                    }
                    Sound_PlaySong(0x21E);
                }
            }
        } else if (requested == 1) {
            u32 mask = 0x40;
            u32 *flags;
            u32 active;
            u32 *savedFlags;
            flags = &state->flags3D4;
            active = *flags & mask;
            savedFlags = flags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (!active) {
                u32 hidden = SceneActorVisibility_IsHidden(actor);
                if (!hidden) {
                    state->visibilityFrame = 0;
                    {
                        u32 mask = ~0x30;
                        {
                            u32 old = *savedFlags & mask;
                            *savedFlags = old | 0x40;
                        }
                    }
                    Sound_PlaySong(0x21E);
                }
            }
        } else {
            ActorStatus_SetDrawableFlags(actor, 1);
            ActorShadow_Disable(actor->base.shadow);
        }
    }
}

u32 Trigger_ConsumePending(void *, u32);

// confidence=low: homes on a stored point, then consumes a pending trigger and restarts.

void SceneActorControl_UpdateWanderTarget(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        struct SceneActorControlMotionState *context = actor->base.context;
        context->field3E0 = 0;
        context->swayStep = 1;
        SceneActorControl_StartStoredCurve(actor);
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&context->vectors[2]);
        SceneActorPlacement_SetFlags(0x800, &actor->base);
    }
    SceneActorControl_TickSway(actor);
    {
        s32 speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
        if (ApproachStoredPoint(actor, (struct StoredCurve *)body->unknown3F0, speed)) {
            if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
                Trigger_ConsumePending(actor->base.receiver, 1);
                SceneActorTerrain_EnterState(actor, 2, 0, 2);
            }
        } else {
            actor->base.counter2AC++;
        }
    }
}

void SceneActorControl_ConsumePendingEntry(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        Trigger_ConsumePending(actor->base.receiver, 1);
        SceneActorPlacement_SetFlags(0x800, &actor->base);
    }
    actor->base.counter2AC++;
}

extern u8 gUnk_08605C64[];

void SceneActorControl_SetStoredMotionCallbacks(u8 *p)
{
    *(u8 **)(p + 0x29c) = gUnk_08605C64;
}

void SceneActorControl_TickHiddenResponse(u8 *);

void SceneActorControl_DispatchControl(u8 *);

u32 Bat_OnUpdate(struct RoomActor *p)
{
    SceneActorControl_TickHiddenResponse((void *)p);
    Callback_InvokeActor(p, p->stateCallbacks[p->state2A8]);
    SceneActorControl_DispatchControl((void *)p);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 SceneActorControl_BindPrimaryPlayer(struct RoomActor *p)
{
    p->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActor_UpdateActiveCallbacks((void *)p);
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

u32 Bat_Destroy(struct RoomActor *s)
{
    u8 *p = s->drawable;
    struct SceneActorBody *q;
    u32 m;
    u32 v;
    if (s->drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(p + 0x20);
    Heap_Free(p);
    m = 0x100;
    if (SceneActorChase_TestField(&s->placementFlags, m))
        SceneMessages_UnregisterReceiver(s->receiver);
    {
        u8 *r = s->shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = s->statusFlags & m;
    q = &s->body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&s->colliderA);
    SceneActorRegistry_UnlinkActor(s);
    return 0;
}
