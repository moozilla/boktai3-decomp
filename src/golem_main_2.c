#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "golem.h"
#include "scene_actor_chase_reaction.h"
#include "player.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_reset.h"
#include "scene_body_api.h"
#include "render.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_dispatch.h"
#include "scene_actor_init_graphics.h"

/* Golem: route back to its home cell; near it, settle (Golem_AlignChargeTarget), else face the cell centre; then go to state 2 (tentative) */

u8 Golem_InvokeEnabledBehavior(struct ChaseActor *actor);

void Golem_BeginChargeBuildup(void);

void Golem_UpdateRouteMotion(void);

void Golem_CenterPositionInCell(struct ContactPosition *);

void Golem_AlignChargeTarget(void *);

s32 Math_Atan2(s32, s32);

static inline u8 TakeFlag2B1(struct ChaseActor *actor)
{
    if (actor->base.field2B1) {
        actor->base.field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void Golem_ReturnHome(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Golem *body = actor->base.context;
    struct ContactPosition center;
    struct ContactPosition diff;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_BeginChargeBuildup;
        SetControl(actor, 0x25, target);
        actor->base.body.direction = actor->base.tracking.heading;
        actor->base.stepDone = 0;
        body->charge688 = 0;
    }
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->base.stepDone) {
        struct ContactPosition *dest = group->destination;
        struct SceneActorRegistryRoute *link = &group->homeRoute;
        SceneActorChase_SnapToRoute(dest, link);
        (group->destination)->pad = 0;
        body->field68D = 0;
        body->routeDelay = 0x78;
        body->counter686 = 0;
        body->field684 = 2;
        body->pushSpeed = 0;
        body->target69C = body->home694;
        SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
        Golem_CenterPositionInCell(&center);
        SceneActorTerrain_Subtract(&diff, &actor->base.body.position, &center);
        if (diff.x * diff.x + diff.z * diff.z < 0x400)
            Golem_AlignChargeTarget(body);
        else {
            u8 angle;
            ((struct ChaseRouteProbe *)group->route)->destination = center;
            SceneActorTerrain_Subtract(&diff, &center, &actor->base.body.position);
            angle = ((Math_Atan2(diff.x, diff.z) + 0x20) & 0xFF) >> 6 << 6;
            actor->base.body.direction = angle;
            body->direction68E = angle;
        }
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        SceneActorChase_SetPhase(actor, 2, 1);
    } else
        actor->base.counter2AC++;
}

void Golem_UpdateChargeBuildup(void);

void Golem_UpdateChargeSettle(struct ChaseActor *actor)
{
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateChargeBuildup;
        SceneActorChase_SetTypedControl(actor, target, 0x22);
        actor->base.stepDone = 0;
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->base.stepDone) {
        {
            void *target = Golem_UpdateRouteMotion;
            SceneActorChase_SetTypedControl(actor, target, 0x23);
        }
        SceneActorChase_SetPhase(actor, 2, 1);
    } else
        actor->base.counter2AC++;
}

// confidence=low: existing Golem state interpretation retained.

void SceneActor_AccelerateForTerrain(struct ChaseActor *);

u8 Actor_StepTowardTargetAxis(struct ChaseActor *, void *, u32, u32, s32, u32);

s32 SceneActorTerrain_CompareNeighborHeight(s32, u32, u32);

void Skeleton_StepPosition(struct ContactPosition *, u32);

void SceneActorReaction_SlideBack(void);

void Golem_UpdateChargeStop(void);

void Golem_UpdateChargeRecovery(void);

struct GolemChargeBounds {
    u8 unknown0[0x1C];
    struct ContactPosition home;
};

static inline u8 Golem_HasRouteFlag(const struct Golem *body, u32 flag)
{
    return SceneActorChase_Test(body->flags628, flag);
}

static inline u32 IsDazzled(struct Player *player)
{
    return SceneActorChase_Bits32(&player->statusFlags, 0x20000);
}

static inline s32 Wrap(s32 d)
{
    if (d > 128)
        d -= 256;
    else if (d < -128)
        d += 256;
    return d;
}

/* Start behavior BEHAVIOR of the current state on the next update. */

static inline void Golem_SetBehavior(void *object, u32 behavior)
{
    struct ChaseActor *actor = object;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

/* Turned too far off the charge heading: face the new direction and spend 10 stamina; out of
 * stamina, stop the charge (behavior 3), else aim at the route's home and recover (behavior 2). */

static inline void Golem_TurnAway(struct Golem *body)
{
    body->direction691 = body->savedDirection690;
    ((struct ChaseActor *)body)->base.body.direction = body->savedDirection690;
    body->routeDelay -= 10;
    if (body->routeDelay <= 0) {
        SceneActorChase_SetControl(body, Golem_UpdateChargeStop, 0x27);
        Golem_SetBehavior(body, 3);
        return;
    }
    body->target69C = ((struct GolemChargeBounds *)((struct ChaseActor *)body)->group.route)->home;
    {
        void *target = Golem_UpdateChargeRecovery;
        SceneActorChase_SetControl(body, target, 0x26);
    }
    Golem_SetBehavior(body, 2);
}

static inline void Golem_Recover(struct Golem *body)
{
    body->routeDelay -= 30;
    body->direction691 = body->savedDirection690;
    SceneActorChase_SetState((struct ChaseActor *)body, 0, 2, 7);

    {
        void *target = SceneActorReaction_SlideBack;
        SceneActorChase_SetControl(body, target, 0x2B);
    }
}

static inline s32 Golem_TurnGap(u8 desired, const u8 *facing)
{
    s32 d = Wrap(desired - *facing);
    if (d < 0)
        d = -d;
    return d;
}

static inline void Golem_Retarget(struct ChaseActor *actor, struct Golem *body)
{
    s32 d = Wrap(actor->base.tracking.heading - body->direction68E);
    if ((d < 0 ? -d : d) <= 15 && actor->base.terrainCell != actor->base.previousTerrainCell) {
        body->target69C = body->home694;
        Golem_AlignChargeTarget(body);
    }
}

static inline void SetCommand(struct SceneActorReactionCommand *command, void *payload, u32 kind,
                              u32 duration)
{
    command->duration = duration;
    command->payload = payload;
    command->first = 0;
    command->second = 0;
    command->kind = kind;
}

static inline s32 Golem_DifferenceHeading(const struct ContactPosition *v)
{
    return Math_Atan2(v->x, v->z);
}

/* Charge pursuit (tentative): slide back to recover when the target player is dazzled or the
 * charge reaches its end; turning more than 31 off the charge heading costs stamina
 * (Golem_TurnAway); out of stamina, restore control after a delay and chase the player or walk
 * back along the route. */

void Golem_UpdateChargePursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Golem *body = actor->base.context;
    struct ContactPosition diff;
    void *route;
    if (TakeFlag2B1(actor))
        body->flag625 = 1;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (body->flag625 == 1 && body->pushSpeed == 0) {
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
        body->flag625 = 0;
    }
    SceneActor_AccelerateForTerrain(actor);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *partner = actor->base.tracking.target;
        s32 index;
        if (!partner)
            goto route;
        index = SceneActorChase_PlayerSlot(partner);
        if (index < 0)
            goto route;
        if (IsDazzled(gPlayers[index])) {
            u8 held = Golem_HasRouteFlag(body, 0x80000);
            if (!held) {
                Golem_Recover(body);
                return;
            }
        }
    } else if (IsDazzled(gPlayers[0])) {
        u8 held = Golem_HasRouteFlag(body, 0x80000);
        if (!held) {
            Golem_Recover(body);
            return;
        }
    }
route:
    route = group->route;
    {
        u16 flag = *(u16 *)route & 4;
        if (flag) {
            if (Actor_StepTowardTargetAxis(actor, route, 24, 24, body->pushSpeed, 0)) {
                s32 distance;
                s32 d;
                body->target69C = body->home694;
                SceneActorTerrain_Subtract(&diff, &body->target69C, &actor->base.body.position);
                distance = diff.x * diff.x + diff.z * diff.z;
                {
                    u8 *facing = &body->direction68E;
                    body->flag625 = 1;
                    d = Wrap(actor->base.tracking.heading - *facing);
                }
                if (d < 0)
                    d = -d;
                if (d <= 15 && distance <= 0xFFFE && body->pushSpeed > 199) {
                    Golem_Recover(body);
                    return;
                }
                {
                    u32 facing = body->direction68E;
                    s32 a = Wrap(Golem_DifferenceHeading(&diff) - facing);
                    a = a < 0 ? -a : a;
                    if (a <= 15) {
                        u32 cell = SceneActorChase_CellAt(&body->target69C);
                        u32 height = *SceneActorTerrain_GetCell(cell) & 15;
                        u32 direction = ((body->direction68E + 0x20) & 0xFF) >> 6;
                        if (!SceneActorTerrain_CompareNeighborHeight(cell, height, direction))
                            Skeleton_StepPosition(&body->target69C, direction);
                        Golem_AlignChargeTarget(body);
                    } else if ((u32)(a - 0x34) <= 0x18)
                        Golem_AlignChargeTarget(body);
                }
                if (Golem_TurnGap(body->savedDirection690, &body->direction68E) > 31) {
                    Golem_TurnAway(body);
                    return;
                }
                SceneActorTerrain_Subtract(&diff, &actor->base.tracking.target->position,
                                           &actor->base.body.position);
                distance = diff.x * diff.x + diff.z * diff.z;
                if (distance <= 0xFFFE && body->pushSpeed > 199) {
                    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
                        struct SceneActorChaseTarget *partner = actor->base.tracking.target;
                        s32 index;
                        if (!partner)
                            goto recover;
                        index = SceneActorChase_PlayerSlot(partner);
                        if (index < 0)
                            goto recover;
                        if (IsDazzled(gPlayers[index]))
                            goto recover;
                        body->charge688 = 400;
                    } else {
                        if (IsDazzled(gPlayers[0]))
                            goto recover;
                        body->charge688 = 400;
                    }
                }
            recover:
                Golem_Recover(body);
                return;
            }
            Golem_Retarget(actor, body);
            if (Golem_TurnGap(body->savedDirection690, &body->direction68E) > 31) {
                Golem_TurnAway(body);
                return;
            }
        } else {
            if (Actor_StepTowardTargetAxis(actor, route, 24, 24, body->pushSpeed, 0)) {
                body->target69C = body->home694;
                Golem_AlignChargeTarget(body);
            } else
                Golem_Retarget(actor, body);
            if (Golem_TurnGap(body->savedDirection690, &body->direction68E) > 31) {
                Golem_TurnAway(body);
                return;
            }
        }
    }
    if (body->routeDelay > 0)
        goto advance;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SelectState(actor, 3, 1, 0);
    SceneActorChase_ResetStateEntry(actor);
    {
        void *target = SceneActorReaction_RestoreAfterDelay;
        SceneActorChase_SetControl(actor, target, 0xE);
    }
    {
        u16 *flags = SceneActorChase_MotionFlagsOf(actor);
        s32 mask = ~0x100;
        *flags = mask & *flags;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SetCommand((void *)&group->saved, group->common.gate.control, 12, 30);
        if (!SceneActorChase_Arrive(actor))
            return;
        actor->base.field2C2 = 0x20;
        goto advance;
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        actor->base.field2C2 = 0x40;
        SetCommand((void *)&group->saved, actor->base.onAlternateEntry, 7, 60);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->pad = 1;
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
advance:
    body->counter686++;
    actor->base.counter2AC++;
}

/* Golem: build up charge while the target player is not dazzled; charge at the partner when ready and in reach (kind 0x28), else approach (kind 0x23) (tentative) */

u8 sub_0821D4F0(void *, void *, u32, u32);

void Golem_UpdateChargeWindup(void);

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

/* Player slot of an actor (byte +8), or -1 when it is not a player (> 3). */

/* Charge up while the target player is not dazzled (flag 0x20000): the target is the partner's
 * player slot in co-op modes, else player 0. The charge grows faster the closer the body. */

static inline void Golem_Charge(void *context)
{
    struct Golem *body = context;
    struct Player *player;
    if (Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *partner = body->chase.base.tracking.target;
        s32 index;
        if (!partner)
            goto charge;
        index = SceneActorChase_PlayerSlot(partner);
        if (index < 0)
            goto charge;
        player = gPlayers[index];
    } else
        player = gPlayers[0];
    if (IsDazzled(player))
        return;
charge: {
    if (SceneActorChase_QueryMotionFlags((struct ChaseActor *)body, 0x80)) {
        s32 gain = 0x400000 - body->chase.base.tracking.distance;
        u16 *charge;
        if (gain < 0)
            gain = 0;
        charge = &((struct Golem *)context)->charge688;
        *charge += gain >> 16;
        if (*charge > 400)
            *charge = 400;
    }
}
}

/* Can the body charge at its partner: enough stored charge, closer than its range, in reach? */

static inline s32 CanCharge(void *ctx)
{
    u8 *body = ctx;
    u8 *pos = body + 0x50;
    u8 ready;
    if (!*(void **)(body + 0xDC))
        return 0;
    ready = 0;
    if (*(u16 *)(body + 0x1E6) == 0) {
        if (*(u16 *)(body + 0x688) > 399)
            ready = 1;
    } else if (*(u16 *)(body + 0x688) > 199)
        ready = 1;
    if (ready && *(u32 *)(body + 0xF0) < *(u32 *)(body + 0xF8) &&
        sub_0821D4F0(pos + 0xC, *(u8 **)(body + 0xDC) + 0xC, 10, 256))
        return 1;
    return 0;
}

void Golem_ChargeOrApproach(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    if (TakeFlag2B1(actor))
        actor->base.stepDone = 0;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->base.stepDone) {
        Golem_Charge(body);
        if (CanCharge(body)) {
            void *target = Golem_UpdateChargeWindup;
            SetControl(actor, 0x28, target);
            SceneActorChase_SetPhase(actor, 2, 4);
            return;
        }
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        SceneActorChase_SetPhase(actor, 2, 1);
    } else
        actor->base.counter2AC++;
}

u8 SceneActorTarget_TrySelectNearestParticipant(struct ChaseActor *);

static inline void Golem_SetRouteFlags(u32 *field, u32 bits)
{
    *field = bits | *field;
}

/* Restore pursuit or a home route when the active control completes. */

void Golem_ResumeRouteAfterControl(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    SceneActorControl_TakeStateEntry(actor);
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->base.stepDone) {
        u16 blocked;
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        ProximityVolume_TranslateBounds(group->route, &actor->base.body.position);
        blocked = SceneActorChase_MotionFlags(actor, 0x80);
        if (blocked) {
            SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
            if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(actor))
                    return;
            } else
                SceneActorChase_TrackPlayer(actor);
            if (SceneActorChase_Arrive(actor))
                actor->base.field2C2 = 0x20;
        } else {
            u32 bit = 0x200;
            u16 returning = SceneActorChase_MotionFlags(actor, bit);
            u32 *flags = &actor->base.behaviorFlags;
            if (returning) {
                struct SceneActorRegistryControl *control = &group->common.control;
                if (SceneActorChase_Bits32(flags, 0x200000)) {
                    SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x1000000);
                    SceneActorChase_ClearMotionFlagComponent(actor, bit);
                    control->mode = 0;
                    control->counter = 0;
                    control->timer = 0;
                }
                SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
            }
            Golem_SetRouteFlags(flags, 0x10);
            actor->base.field2C2 = 0x40;
            SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 60);
            SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
            group->destination->pad = 1;
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          group->destination))
                Callback_InvokeActor(actor, group->fallback);
        }
    } else
        actor->base.counter2AC++;
}

struct Golem080CEAC4Body {
    u8 unknown0[9];
    u8 direction;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void SetPhase(struct Golem *actor, u32 state, u32 phase, u32 behavior)
{
    actor->chase.base.state2A8 = state;
    actor->chase.base.phase2A9 = phase;
    actor->chase.base.behavior2AA = behavior;
    actor->chase.base.counter2AC = 0;
    actor->chase.base.field2B1 = 1;
}

void Golem_UpdateHoldTimer(struct Golem *actor)
{
    struct Golem *context = actor->chase.base.context;
    struct Golem080CEAC4Body *body = (struct Golem080CEAC4Body *)((u8 *)actor + 0x50);
    TakeFlag(&actor->chase.base.field2B1);
    if (Golem_InvokeEnabledBehavior(&actor->chase))
        return;
    body->direction = actor->chase.base.tracking.heading;
    if ((s32)actor->chase.base.counter2AC > 0x77) {
        SetPhase(actor, 0, 2, 5);
        context->charge688 = 0;
    }
    actor->chase.base.counter2AC++;
}

/* Golem_BeginSlideState: on entry hands control to the slide state (Golem_UpdateSlideImpact, kind 0x29), faces the stored heading and sets the context push speed to 0x200; after 30 frames or once the control finishes, reports the pending context event and returns to the idle control (Golem_UpdateChargeStop, kind 0x27) in phase 2/3 (provisional name) */

void Golem_UpdateSlideImpact(void);

void Golem_BeginSlideState(struct ChaseActor *actor)
{
    struct Golem *context = actor->base.context;
    struct SceneActorBody *body = &actor->base.body;
    if (TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetTypedControl(actor, Golem_UpdateSlideImpact, 0x29);
        actor->base.stepDone = 0;
        body->direction = actor->base.tracking.heading;
        context->pushSpeed = 0x200;
    }
    if ((s32)actor->base.counter2AC > 0x1E || actor->base.stepDone) {
        if (context->pendingEvent) {
            actor->base.counter304 = context->pendingEvent;
            Callback_InvokeActor(actor, actor->base.targetQuery);
        }
        SceneActorChase_SetTypedControl(actor, Golem_UpdateChargeStop, 0x27);
        SceneActorChase_SetPhase(actor, 2, 3);
    } else {
        actor->base.counter2AC++;
    }
}

void Golem_UpdateRecoil(void);

void Golem_UpdateSettle(struct ChaseActor *actor)
{
    struct Golem *body = actor->base.context;
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateRecoil;
        SceneActorChase_SetTypedControl(actor, target, 0x2A);
        actor->base.stepDone = 0;
        SceneActorChase_SetActorFlags(actor, 0x2000);
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->base.stepDone) {
        body->target69C = body->home694;
        Golem_AlignChargeTarget(body);
        {
            void *target = Golem_UpdateRouteMotion;
            SceneActorChase_SetTypedControl(actor, target, 0x23);
        }
        SceneActorChase_SetPhase(actor, 2, 1);
    } else
        actor->base.counter2AC++;
}

/* Golem_ChargeOrReturnHome: builds up charge and cools the charge timer; if the target player is dazzled goes idle (kind 0x27), else charges at the partner when ready (kind 0x28); otherwise walks back to the centre of its home cell, idling there once within reach (tentative) */

/* Player slot of an actor (byte +8), or -1 when it is not a player (> 3). */

/* Charge up while the target player is not dazzled (flag 0x20000): the target is the partner's
 * player slot in co-op modes, else player 0. The charge grows faster the closer the body. */

/* Can the body charge at its partner: enough stored charge, closer than its range, in reach? */

void Golem_ChargeOrReturnHome(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Golem *body = actor->base.context;
    u8 coop;
    if (TakeFlag2B1(actor))
        actor->base.stepDone = 0;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->base.stepDone) {
        Golem_Charge(body);
        body->routeDelay -= 0x1E;
        coop = Test(gEngineFlags, 0x4800);
        if (coop) {
            struct SceneActorChaseTarget *partner = actor->base.tracking.target;
            s32 index;
            if (!partner)
                goto check;
            index = SceneActorChase_PlayerSlot(partner);
            if (index < 0)
                goto check;
            {
                struct Player *player = gPlayers[index];
                if (!IsDazzled(player))
                    goto check;
            }
            SceneActorChase_SetTypedControl(actor, Golem_UpdateChargeStop, 0x27);
            SceneActorChase_SetPhase(actor, 2, 3);
            return;
        } else {
            struct Player *player = gPlayers[0];
            if (!IsDazzled(player))
                goto check;
            SceneActorChase_SetTypedControl(actor, Golem_UpdateChargeStop, 0x27);
            SceneActorChase_SetPhase(actor, 2, 3);
            return;
        }
    check:
        if (body->routeDelay <= 0) {
            if (!CanCharge(body)) {
                SceneActorChase_SetTypedControl(actor, Golem_UpdateChargeStop, 0x27);
                SceneActorChase_SetPhase(actor, 2, 3);
                return;
            }
        } else if (!CanCharge(body))
            goto approach;
        SceneActorChase_SetTypedControl(actor, Golem_UpdateChargeWindup, 0x28);
        SceneActorChase_SetPhase(actor, 2, 4);
        return;
    approach: {
        struct ContactPosition center, delta;
        SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
        Golem_CenterPositionInCell(&center);
        SceneActorTerrain_Subtract(&delta, &actor->base.body.position, &center);
        if (delta.x * delta.x + delta.z * delta.z <= 0xFFF) {
            Golem_AlignChargeTarget(body);
            SceneActorChase_SetPhase(actor, 2, 6);
            return;
        }
        ((struct ChaseRouteProbe *)group->route)->destination = center;
        SceneActorTerrain_Subtract(&delta, &center, &actor->base.body.position);
        {
            u8 direction = ((Math_Atan2(delta.x, delta.z) + 0x20) & 0xFF) >> 6 << 6;
            actor->base.body.direction = direction;
            body->direction68E = direction;
        }
        SceneActorChase_SetTypedControl(actor, Golem_UpdateRouteMotion, 0x23);
        SceneActorChase_SetPhase(actor, 2, 1);
    }
    } else
        actor->base.counter2AC++;
}

/* This class's own copies of the shared chase inlines it compiles differently. */

static inline s32 Golem_CellAt(struct ContactPosition *position)
{
    return SceneActorChase_CellIndex(position->x >> 8, position->z >> 8);
}

static inline u32 Golem_Height(struct ContactPosition *position)
{
    return SceneActorChase_HeightAt(Golem_CellAt(position), position);
}

static inline void Golem_SnapToRoute(struct ContactPosition *position,
                                     struct SceneActorRegistryRoute *route)
{
    position->x = route->point->x;
    position->z = route->point->z;
    position->y = Golem_Height(position);
}

void Skeleton_ClearDirectionalEntryFlag(void);

static inline u8 Golem_ArriveAndRestoreRoute(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    Golem_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            callback = &group->fallback;
            goto invoke;
        }
        cell = ((struct SceneActorChasePlayer *)gPlayers[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    SceneActorChase_ClearFlagComponent((void *)&group->common.gate.flags, 4);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        SceneActorChase_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorChase_SetBehavior(actor, 5);
        Golem_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y =
            SceneActorChase_HeightAt(Golem_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorTerrain_GetCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
invoke:
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

/* Resume the saved response, then select the arrival or reroute operation. */

void Golem_ArriveAndRestore(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SceneActorChase_SetControl(actor, Skeleton_ClearDirectionalEntryFlag, 0x2C);
        actor->base.stepDone = 0;
    }
    if (!actor->base.stepDone)
        return;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80))
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 120);
    else
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 120);
    SceneActorChase_SetActorFlags(actor, 0x10);
    actor->base.field2C2 = 0x20;
    SceneActorChase_SetState(actor, 3, 1, 0);
    Golem_ArriveAndRestoreRoute(actor);
}

extern const u8 gUnk_0860613C[];

struct S {
    u8 filler[652];
    const u8 *p;
};

void Golem_InitSpecialActionHandlers(struct S *s)
{
    s->p = gUnk_0860613C;
}

void Golem_TickFacingQuadrant(void *);

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

void Golem_UpdateEffectsAndContacts(void *);

struct SOnUpdate {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 Golem_OnUpdate(struct SOnUpdate *s)
{
    u32 m;
    Golem_TickFacingQuadrant(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        Callback_InvokeActor(s, s->tbl[s->idx]);
        SceneActorControl_DispatchControl(s);
    }
    Golem_UpdateEffectsAndContacts(s);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(void);

s32 Golem_OnSharedActorUpdate(void)
{
    SceneActor_UpdateBehaviorCallbacks();
    return 0;
}

void SceneActorEffects_ReleaseResources(void);

s32 Golem_OnDestroy(u8 *unused)
{
    SceneActorEffects_ReleaseResources();
    return 0;
}

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_SetMotionMode(void);

u32 SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

u32 Route_GetCurrentPointFacing(void *);

s32 TargetRegistry_RegisterActor(void *, void *, void *);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

s32 Anim_Step(void *, void *);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorMovement_ResolveRoutePosition(void *);

u8 SceneActorTarget_UpdateTrackedContact(void *);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

static inline void GolemInit_ResetFlags(void *object)
{
    struct Golem *actor = object;
    actor->flags628 = 0;
}

static inline void GolemInit_InitializeBody(void *object, u32 *fill)
{
    struct Golem *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    struct SceneActorBody *body = &arg0->chase.base.body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, identity;
    struct SceneActorRegistryRoute *rt;
    arg0->chase.base.behaviorFlags = 0;
    arg0->chase.base.statusFlags = 0;
    arg0->chase.base.enabled = 0;
    arg0->chase.base.field2E8 = 8;
    arg0->chase.base.placementFlags = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->chase.base.field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadAttacks(&arg0->chase);

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
    MonsterInit_ReadTargeting(&arg0->chase.base.tracking);
    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(&arg0->chase, 0x200);
    arg0->chase.base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->chase.base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(&arg0->chase, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(&arg0->chase));
        arg0->chase.base.field9E = MonsterInit_ReadByte(0);
        arg0->chase.base.defense = MonsterInit_ReadByte(0);
        arg0->chase.base.contactReward = MonsterInit_ReadByte(0);
        arg0->chase.base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->chase.base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(&arg0->chase, MonsterInit_GetInitialHealth(&arg0->chase),
                                 firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->chase.base.experience = MonsterInit_ReadWord(0);
        arg0->chase.base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->chase.base.hudKind = MonsterInit_ReadKeywordHalfword('K', 2);
    identity = (s16)Script_GetKeywordValue(0x56, -1);
    if (identity < 0)
        identity = 0xFFFF;
    arg0->chase.base.hudLabel = identity;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(&arg0->chase, 0x800);
    if (Script_GetKeywordValue(0x4C, 0))
        MonsterInit_SetStatusFlags(&arg0->chase, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(&arg0->chase, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->chase.base.body.position, rt);
        MonsterInit_DrawablePosition(&arg0->chase);
        arg0->chase.base.body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(&arg0->chase, 0x200))
        TargetRegistry_RegisterActor(&group->common.placement, body,
                                     SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(
        body, MonsterInit_GetDrawableKind(&arg0->chase) == 0 ? arg0->chase.base.drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, arg0->chase.base.movementProbe, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, arg0->chase.base.movementProbe, 0x40, 0x40);

    group->common.bodyId = arg0->chase.base.body.id;
    group->common.bodyPosition = &arg0->chase.base.body.position;
}

void Golem_InitAnimationProfile(void *);

void Golem_InitColliders(void *);

void Golem_InitActionHandlers(void *);

void Golem_InitSpecialActionHandlers(struct S *s);

s32 Golem_OnUpdate(struct SOnUpdate *s);

u16 Skeleton_LookupAnimationEntry(void *);

void Golem_TurnTowardRouteFacing(void *);

void Golem_UpdateIdleState(void *);

void Golem_UpdateIdle(void *);

void Golem_ResumeBehaviorAfterDisplacement(void *);

void Golem_UpdateDefeat(void *);

void Golem_UpdateRevivalReturn(void *);

void SceneActorTarget_ResolvePendingObject(void);

void Golem_UpdateDying(void);

void Golem_ResumeSavedTarget(void);

void Golem_OnUnusedState(void);

void Golem_ResetCombatState(void);

void SceneActorControl_RefreshMotionRoute(void);

u8 Golem_UpdateHitReaction(void *);

void Golem_ClearControlFlag(void);

void Golem_RestoreRoute(void);

s32 Golem_OnSharedActorUpdate(void);

s32 Golem_OnDestroy(u8 *);

static inline void GolemInit_EnableInitialFlags(struct Golem *actor)
{
    MonsterInit_SetTargetingFlags(&actor->chase.base.tracking, 1);
    MonsterInit_SetBehaviorFlags(&actor->chase, 0x1600000);
    MonsterInit_SetStatusFlags(&actor->chase, 0x8000008);
}

static inline struct Golem *GolemInit_Context(struct Golem *actor)
{
    return actor->chase.base.context;
}

static inline void GolemInit_InitializeReceiver(struct Golem *actor)
{
    MonsterInit_ClearReceiver(&actor->chase);
    GolemInit_Context(actor)->rangeSquared62C = actor->chase.base.tracking.range;
}

static inline void GolemInit_InstallCompletionCallbacks(struct Golem *arg0)
{
    arg0->chase.base.onMessage = SceneActorScript_OnCommand;
    arg0->chase.base.update = (SceneActorResultCallback)Golem_OnSharedActorUpdate;
    arg0->chase.base.destroy = (SceneActorResultCallback)Golem_OnDestroy;
}

static inline void GolemInit_InitializePaletteRange(struct Golem *actor, u32 alternate, u32 current)
{
    actor->chase.base.alternatePalette = MonsterInit_GetPaletteBase(&actor->chase) + alternate;
    actor->chase.base.paletteBank = MonsterInit_GetPaletteBase(&actor->chase) + current;
}

s32 Golem_Init(void *object)
{
    struct Golem *arg0 = object;
    struct Golem *actor = object;
    struct SceneActorChaseGroup *group = &arg0->chase.group;
    // concession-ok(const-local): CpuSet reads this fixed source word for both 8-byte fills.
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;

    GolemInit_ResetFlags(arg0);
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_GOLEM);
    Golem_InitAnimationProfile(actor);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0) {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->chase.base.drawable, 0x5C);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_InitializeSprite(heap, 0x5290, 0x1E38);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->chase.base.body.direction = 0;
    } else {
        u8 *heap;
        actor->chase.base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->chase.base.drawable, 0x80);
        heap = MonsterInit_Drawable(&actor->chase);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5290, &tmp);
        actor->chase.base.animationIndex = 0xFF;
    }
    GolemInit_InitializeBody(arg0, &zero);
    MonsterInit_ReadPalette(&actor->chase, 0x27D, 0x192);
    GolemInit_InitializePaletteRange(actor, 2, 0);
    Golem_InitColliders(actor);
    MonsterInit_InstallSequenceCallbacks(
        &actor->chase, Golem_OnUpdate, Skeleton_LookupAnimationEntry, Golem_TurnTowardRouteFacing,
        Golem_UpdateIdleState, Golem_UpdateIdle, Golem_UpdateRevivalReturn,
        Golem_ResumeBehaviorAfterDisplacement, Golem_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, SceneActorTarget_UpdateTrackedContact,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        SceneActorPlacement_ResetStateThree);
    MonsterInit_InitializeGroupWithBounds(
        group, &arg0->routeProbe, &arg0->destination, Golem_UpdateDying, Golem_ResumeSavedTarget,
        SceneActorReaction_BeginTargetRecovery, Golem_OnUnusedState, 1, &actor->chase.base.body);
    SceneActorMovement_ResolveRoutePosition(actor);
    MonsterInit_SetRouteReset(&actor->chase, Golem_ResetCombatState);
    MonsterInit_SetReactionGate(&actor->chase, SceneActorControl_RefreshMotionRoute);
    MonsterInit_SetContactCallback(&actor->chase, Golem_UpdateHitReaction);
    group->responseCallback = Golem_ClearControlFlag;
    group->routeRestoreCallback = Golem_RestoreRoute;
    actor->chase.base.field2E8 = 0;
    GolemInit_EnableInitialFlags(actor);
    MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~1);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    MonsterInit_ReadInitialMode(&actor->chase);
    flag = 0;
    if (MonsterInit_TestMotionFlags(&actor->chase, 4)) {
        MonsterInit_SetStatusFlags(&actor->chase, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(&actor->chase, 0, 0, 0);
        MonsterInit_SetControl(&actor->chase, MonsterInit_GetStandingControl(&actor->chase), 0);
    }
    MonsterInit_InitializeRegion(&actor->chase);
    if (MonsterInit_TestMotionFlags(&actor->chase, 0x40))
        MonsterInit_MaskColliderFlags(&actor->chase.base.colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(&actor->chase))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(&actor->chase) == 0)
        DisplaySequence_Step(actor->chase.base.drawable);
    else
        Anim_Step(actor->chase.base.drawable + 0x20, actor->chase.base.drawable);
    GolemInit_InitializeReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Golem_InitActionHandlers(actor);
    Golem_InitSpecialActionHandlers(actor);

    {
        struct Golem *instance = arg0;
        instance->field68D = 0;
        instance->routeDelay = 0x78;
        instance->counter686 = 0;
        instance->field684 = 2;
        instance->pushSpeed = 0;
        instance->routeDelay = 0xA0;
        instance->field68A = 0;
        instance->responseCount = 0;
        instance->field693 = 0;
    }

    MonsterInit_SaveControl(MonsterInit_SavedControlOf(&actor->chase), &actor->chase);
    GolemInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Golem_Init(void *object);

void Heap_Free(void *);

void Golem_Create(void)
{
    u8 *p = Heap_Alloc(0x6b0);
    if (p != 0) {
        Memory_Clear(p, 0x6b0);
        if (Golem_Init(p) < 0) {
            Golem_OnDestroy(p);
            Heap_Free(p);
        }
    }
}
