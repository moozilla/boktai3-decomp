#include "global.h"
#include "scene_actor_control_helpers.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
#include "scene_actor_availability.h"
#include "scene_actor_terrain_reaction.h"
#include "scene_actor_visibility.h"
#include "scene_actor_state.h"
#include "scene_actor_placement.h"
#include "scene_actor_status.h"

/* SceneActorControl_ReturnSuccess: Returns constant 1; provisional success stub with no known callers */

s32 SceneActorControl_ReturnSuccess(void)
{
    return 1;
}

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

s32 Callback_InvokeActor(void *, void *);

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

struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

static inline u8 ApproachStoredPoint(struct ChaseActor *actor, struct SavedMotion *saved,
                                     s32 inSpeed)
{
    struct SceneActorBody *motion = &actor->base.body;
    s32 speed = inSpeed;
    s16 dx = saved->position.x - (u16)motion->position.x;
    s16 dz = saved->position.z - (u16)motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x += Shift(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        motion->motion.z += Shift(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

// confidence=low: homes on a stored point, then picks a new point around the player.

void SceneActorControl_WanderAroundPlayer(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u8 *context = actor->base.context;
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(context + 0x42C);
        context[0x3E3] = 1;
    }
    SceneActorControl_TickSway(actor);
    if (ApproachStoredPoint(actor, (void *)(body + 0x3F0),
                            Callback_InvokeActor(actor, actor->base.animationQuery.opaque))) {
        if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
            struct ContactPosition target;
            u32 angle;
            angle = (u8)WanderRand();
            target = *(struct ContactPosition *)(((u8 **)gPlayers)[0] + 0x30);
            target.x += Shift(gUnk_086149C4[angle], 4);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 4);
            SceneActorControl_StartPositionCurve(actor, &target);
            {
                u32 *flags =
                    &((struct SceneActorControlMotionState *)actor->base.context)->flags3D4;
                u32 mask = ~8;
                *flags &= mask;
            }
        }
    } else {
        actor->base.counter2AC++;
    }
}

void SceneActorControl_UpdateTimedFacing(struct ChaseActor *);

void SceneActorControl_ResetCollisionPhase(struct ChaseActor *);

static inline void SetMotionPhase(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline u32 MotionRand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)((index << 1) + RANDOM_TABLE_ADDR);
}

struct StoredMotionState {
    u8 unknown0[0x3D4];
    u32 flags;
    u8 unknown3D8[0xB];
    u8 mode;
    u8 unknown3E4[4];
    u16 timer;
    u8 unknown3EA[6];
    struct SavedMotion saved;
    u8 unknown404[0x28];
    struct ContactPosition scratch;
};

static inline void EnableMotionFlags(u32 mask, struct StoredMotionState *state)
{
    state->flags |= mask;
}

struct PlayerPositionView {
    u8 unknown0[0x30];
    struct ContactPosition position;
};

// confidence=low: waits a timed interval, homes on a stored point, then picks a new point.

void SceneActorControl_UpdateStoredMotion(struct ChaseActor *actor)
{
    struct StoredMotionState *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorTerrain_InstallControl(actor, (void *)SceneActorControl_UpdateTimedFacing, 0x24);
        ZeroFill8(&body->scratch);
        body->mode = 1;
        body->timer = 0;
        {
            u32 angle;
            struct ContactPosition target;
            angle = (u8)MotionRand();
            target = ((struct PlayerPositionView *)gPlayers[0])->position;
            target.x += Shift(gUnk_086149C4[angle], 3);
            target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
            SceneActorControl_InitPositionCurveEndpoints(actor, &target);
            SceneActorControl_AdvancePositionCurve(actor);
            {
                u32 *flags = &body->flags;
                u32 mask = ~8;
                *flags &= mask;
            }
        }
    }
    if (body->timer != 0) {
        u32 timer = body->timer;
        timer--;
        body->timer = timer;
        if ((timer << 16) == 0) {
            SceneActorControl_StartPositionCurve(
                actor, &((struct PlayerPositionView *)gPlayers[0])->position);
            EnableMotionFlags(8, body);
        }
    } else {
        SceneActorControl_TickSway(actor);
        if (ApproachStoredPoint(actor, &body->saved,
                                Callback_InvokeActor(actor, actor->base.animationQuery.opaque))) {
            if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
                struct ContactPosition target;
                u32 angle;
                angle = (u8)MotionRand();
                target = ((struct PlayerPositionView *)gPlayers[0])->position;
                target.x += Shift(gUnk_086149C4[angle], 3);
                target.z += Shift(gUnk_086149C4[(angle + 64) & 255], 3);
                SceneActorControl_InitPositionCurveEndpoints(actor, &target);
                SceneActorControl_AdvancePositionCurve(actor);
            }
        }
        if (actor->base.tracking.distance > 0xFDE8) {
            SceneActorControl_ResetCollisionPhase(actor);
            actor->base.body.direction = actor->base.tracking.heading;
            body->timer = 0x1E;
            return;
        }
    }
    if (actor->base.stepDone) {
        SetMotionPhase(actor, 4);
    } else {
        actor->base.counter2AC++;
    }
}

extern u8 gUnk_08605C5C[];

void SceneActorControl_SetApproachCallbacks(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605C5C;
}

static inline void ShowDrawableAndShadow(struct ChaseActor *actor)
{
    ActorStatus_ClearDrawableFlags(actor, 1);
    ActorShadow_Enable(actor->base.shadow);
}
void SceneActorControl_TickVisibility(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state;
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (unavailable)
        return;
    {
        u32 mask;
        u32 active;
        state = SceneActorControl_GetMotionState(actor);
        mask = 0x60;
        active = SceneActorControl_TestFlags(state->flags3D4, mask);
        if (!active)
            return;
    }
    {
        u32 counter;
        u32 value;
        {
            u16 *field = &state->visibilityFrame;
            counter = (u32)*field << 16;
        }
        value = counter >> 16;
        if (value <= 19) {
            if ((u8)(counter >> 18) & 1)
                ShowDrawableAndShadow(actor);
            else {
                ActorStatus_SetDrawableFlags(actor, 1);
                ActorShadow_Disable(actor->base.shadow);
            }
        } else if (value <= 29) {
            if ((u8)(counter >> 17) & 1)
                ShowDrawableAndShadow(actor);
            else {
                ActorStatus_SetDrawableFlags(actor, 1);
                ActorShadow_Disable(actor->base.shadow);
            }
        } else {
            u32 mask = 0x20;
            u32 *inputFlags;
            u32 *flags;
            u32 active;
            inputFlags = &state->flags3D4;
            active = *inputFlags & mask;
            flags = inputFlags;
            active = SceneActorControl_TestFlags(active, ~0);
            if (active) {
                ActorStatus_ClearDrawableFlags(actor, 1);
                {
                    u32 mask = ~0x20;
                    *flags &= mask;
                }
                ActorShadow_Enable(actor->base.shadow);
            } else {
                ActorStatus_SetDrawableFlags(actor, 1);
                {
                    u32 mask = ~0x40;
                    *flags &= mask;
                }
                ActorShadow_Disable(actor->base.shadow);
            }
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
    {
        u32 *flags = &SceneActorControl_GetMotionState(actor)->flags3D4;
        u32 mask = ~8;
        *flags &= mask;
    }
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

struct WanderTriggerView {
    u8 unknown0[0xC];
    u8 trigger[1];
};

struct WanderMotionState {
    u8 unknown0[0x3E0];
    u16 timer;
    u8 unknown3E2, mode;
    u8 unknown3E4[0xC];
    struct SavedMotion saved;
    u8 unknown404[0x28];
    struct ContactPosition scratch;
};

// confidence=low: homes on a stored point, then consumes a pending trigger and restarts.

void SceneActorControl_UpdateWanderTarget(struct ChaseActor *actor)
{
    struct WanderMotionState *body = actor->base.context;
    if (SceneActorControl_TakeStateEntry(actor)) {
        struct WanderMotionState *context = actor->base.context;
        context->timer = 0;
        context->mode = 1;
        SceneActorControl_StartStoredCurve(actor);
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        ZeroFill8(&context->scratch);
        SceneActorPlacement_SetFlags(0x800, &actor->base);
    }
    SceneActorControl_TickSway(actor);
    {
        s32 speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
        if (ApproachStoredPoint(actor, &body->saved, speed)) {
            if ((u8)SceneActorControl_AdvancePositionCurve(actor)) {
                Trigger_ConsumePending(((struct WanderTriggerView *)actor)->trigger, 1);
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
        Trigger_ConsumePending((void *)((u8 *)actor + 0xC), 1);
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

u32 Bat_OnUpdate(u8 *p)
{
    u32 i;
    u32 *t;
    SceneActorControl_TickHiddenResponse(p);
    i = p[0x2a8];
    t = *(u32 **)(p + 0x27c);
    Callback_InvokeActor(p, t[i]);
    SceneActorControl_DispatchControl(p);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 SceneActorControl_BindPrimaryPlayer(u8 *p)
{
    *(u8 **)(p + 0xdc) = (u8 *)gPlayers[0] + 0x24;
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

static inline u8 tst(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

struct S {
    u8 f0[4];
    u8 *p4;
    u8 b8;
    u8 f1[0x2b7];
    u16 h2c0;
};

u32 Bat_Destroy(struct S *s)
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
