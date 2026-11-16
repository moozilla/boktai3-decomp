#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "golem.h"
#include "scene_actor_chase_reaction.h"
#include "player.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_reset.h"

/* Golem: route back to its home cell; near it, settle (Golem_AlignChargeTarget), else face the cell centre; then go to state 2 (tentative) */

u8 Golem_InvokeEnabledBehavior(struct ChaseActor *actor);

void Golem_BeginChargeBuildup(void);

void Golem_UpdateRouteMotion(void);

void Golem_CenterPositionInCell(struct ContactPosition *);

void Golem_AlignChargeTarget(void *);

s32 Math_Atan2(s32, s32);

struct GolemRoutePoint {
    s16 x, z;
};

struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};

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

static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
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
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

void Golem_ReturnHome(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    u8 *body = actor->base.context;
    struct ContactPosition center;
    struct ContactPosition diff;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_BeginChargeBuildup;
        SetControl(actor, 0x25, target);
        actor->base.body.direction = actor->base.tracking.heading;
        actor->base.stepDone = 0;
        *(u16 *)(body + 0x688) = 0;
    }
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->base.stepDone) {
        struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
        struct GolemRouteLink *link = (struct GolemRouteLink *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(dest, CellIndex(dest));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        body[0x68D] = 0;
        *(u16 *)(body + 0x680) = 0x78;
        *(u16 *)(body + 0x686) = 0;
        *(u16 *)(body + 0x684) = 2;
        *(u16 *)(body + 0x682) = 0;
        *(struct ContactPosition *)(body + 0x69C) = *(struct ContactPosition *)(body + 0x694);
        SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
        Golem_CenterPositionInCell(&center);
        SceneActorTerrain_Subtract(&diff, &actor->base.body.position, &center);
        if (diff.x * diff.x + diff.z * diff.z < 0x400)
            Golem_AlignChargeTarget(body);
        else {
            u8 angle;
            *(struct ContactPosition *)(*(u8 **)(group + 0x1FC) + 0x1C) = center;
            SceneActorTerrain_Subtract(&diff, &center, &actor->base.body.position);
            angle = ((Math_Atan2(diff.x, diff.z) + 0x20) & 0xFF) >> 6 << 6;
            actor->base.body.direction = angle;
            body[0x68E] = angle;
        }
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->base.state2A8 = 0;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = 1;
        }
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
    } else
        actor->base.counter2AC++;
}

void Golem_UpdateChargeBuildup(void);

void Golem_UpdateChargeSettle(struct ChaseActor *actor)
{
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateChargeBuildup;
        SetControl(actor, 0x22, target);
        actor->base.stepDone = 0;
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->base.stepDone) {
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->base.state2A8 = 0;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = 1;
        }
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
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

static inline u32 IsDazzled(struct Player *player)
{
    u32 mask = 0x20000;
    return player->statusFlags & mask;
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
            u32 mask = 0x80000;
            u8 held = SceneActorChase_Test(body->flags628, mask);
            if (!held) {
                Golem_Recover(body);
                return;
            }
        }
    } else if (IsDazzled(gPlayers[0])) {
        u32 mask = 0x80000;
        u8 held = SceneActorChase_Test(body->flags628, mask);
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
        {
            u32 mask = 0x10;
            actor->base.behaviorFlags |= mask;
        }
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

static inline s32 PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}

/* Charge up while the target player is not dazzled (flag 0x20000): the target is the partner's
 * player slot in co-op modes, else player 0. The charge grows faster the closer the body. */

static inline void Golem_Charge(void *ctx)
{
    u8 *body = ctx;
    u8 *player;
    if (Test(gEngineFlags, 0x4800)) {
        u8 *partner = *(u8 **)(body + 0xDC);
        s32 index;
        if (!partner)
            goto charge;
        index = PlayerSlot(partner);
        if (index < 0)
            goto charge;
        player = gPlayers[index];
    } else
        player = gPlayers[0];
    {
        u32 mask = 0x20000;
        if (*(u32 *)(player + 0x20) & mask)
            return;
    }
charge: {
    u32 mask = 0x80;
    if (*(u16 *)(body + 0x2D4) & mask) {
        s32 gain = 0x400000 - *(u32 *)(body + 0xF0);
        u16 *charge;
        if (gain < 0)
            gain = 0;
        charge = (u16 *)((u8 *)ctx + 0x688);
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
            {
                u32 phase = 2, behavior = 4;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = phase;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            return;
        }
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->base.state2A8 = 0;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = 1;
        }
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
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

struct Unk080CEB48Body {
    u8 unknown0[9];
    u8 direction;
};

void Golem_UpdateSlideImpact(void);

static inline void BeginControl(struct ChaseActor *actor, void *control, u8 kind)
{
    u32 zero;
    actor->base.field2B2 = 1;
    zero = 0;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}

void Golem_BeginSlideState(struct ChaseActor *actor)
{
    struct Golem *context = actor->base.context;
    struct Unk080CEB48Body *body = (struct Unk080CEB48Body *)((u8 *)actor + 0x50);
    if (TakeFlag(&actor->base.field2B1)) {
        BeginControl(actor, Golem_UpdateSlideImpact, 0x29);
        actor->base.stepDone = 0;
        body->direction = actor->base.tracking.heading;
        context->pushSpeed = 0x200;
    }
    if ((s32)actor->base.counter2AC > 0x1E || actor->base.stepDone) {
        if (context->pendingEvent) {
            actor->base.counter304 = context->pendingEvent;
            Callback_InvokeActor(actor, actor->base.targetQuery);
        }
        BeginControl(actor, Golem_UpdateChargeStop, 0x27);
        SetPhase(actor, 0, 2, 3);
    } else {
        actor->base.counter2AC++;
    }
}

void Golem_UpdateRecoil(void);

void Golem_UpdateSettle(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    u8 busy;
    if (TakeFlag2B1(actor)) {
        void *target = Golem_UpdateRecoil;
        SetControl(actor, 0x2A, target);
        actor->base.stepDone = 0;
        {
            u32 bit = 0x2000;
            u32 *flags = &actor->base.behaviorFlags;
            *flags |= bit;
        }
    }
    busy = Golem_InvokeEnabledBehavior(actor);
    if (busy)
        return;
    if (actor->base.stepDone) {
        *(struct ContactPosition *)(body + 0x69C) = *(struct ContactPosition *)(body + 0x694);
        Golem_AlignChargeTarget(body);
        {
            void *target = Golem_UpdateRouteMotion;
            SetControl(actor, 0x23, target);
        }
        {
            u32 phase = 2;
            actor->base.state2A8 = 0;
            actor->base.phase2A9 = phase;
            actor->base.behavior2AA = 1;
        }
        actor->base.counter2AC = 0;
        actor->base.field2B1 = 1;
    } else
        actor->base.counter2AC++;
}

/* Golem_ChargeOrReturnHome: builds up charge and cools the charge timer; if the target player is dazzled goes idle (kind 0x27), else charges at the partner when ready (kind 0x28); otherwise walks back to the centre of its home cell, idling there once within reach (tentative) */

/* Player slot of an actor (byte +8), or -1 when it is not a player (> 3). */

/* Charge up while the target player is not dazzled (flag 0x20000): the target is the partner's
 * player slot in co-op modes, else player 0. The charge grows faster the closer the body. */

static inline void SetControl_ChargeOrReturnHome(struct ChaseActor *actor, void *control, u32 kind)
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

static inline void SetPhase_ChargeOrReturnHome(struct ChaseActor *actor, u32 phase, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

/* Can the body charge at its partner: enough stored charge, closer than its range, in reach? */

void Golem_ChargeOrReturnHome(struct ChaseActor *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    u8 *body = actor->base.context;
    u8 coop;
    if (TakeFlag2B1(actor))
        actor->base.stepDone = 0;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->base.stepDone) {
        Golem_Charge(body);
        *(u16 *)(body + 0x680) -= 0x1E;
        coop = Test(gEngineFlags, 0x4800);
        if (coop) {
            u8 *partner = (u8 *)actor->base.tracking.target;
            s32 index;
            if (!partner)
                goto check;
            index = PlayerSlot(partner);
            if (index < 0)
                goto check;
            {
                u8 *player = gPlayers[index];
                u32 mask = 0x20000;
                if (!(*(u32 *)(player + 0x20) & mask))
                    goto check;
            }
            SetControl_ChargeOrReturnHome(actor, Golem_UpdateChargeStop, 0x27);
            SetPhase_ChargeOrReturnHome(actor, 2, 3);
            return;
        } else {
            u8 *player = gPlayers[0];
            u32 mask = 0x20000;
            if (!(*(u32 *)(player + 0x20) & mask))
                goto check;
            SetControl_ChargeOrReturnHome(actor, Golem_UpdateChargeStop, 0x27);
            SetPhase_ChargeOrReturnHome(actor, 2, 3);
            return;
        }
    check:
        if (*(s16 *)(body + 0x680) <= 0) {
            if (!CanCharge(body)) {
                SetControl_ChargeOrReturnHome(actor, Golem_UpdateChargeStop, 0x27);
                SetPhase_ChargeOrReturnHome(actor, 2, 3);
                return;
            }
        } else if (!CanCharge(body))
            goto approach;
        SetControl_ChargeOrReturnHome(actor, Golem_UpdateChargeWindup, 0x28);
        SetPhase_ChargeOrReturnHome(actor, 2, 4);
        return;
    approach: {
        struct ContactPosition center, delta;
        SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
        Golem_CenterPositionInCell(&center);
        SceneActorTerrain_Subtract(&delta, &actor->base.body.position, &center);
        if (delta.x * delta.x + delta.z * delta.z <= 0xFFF) {
            Golem_AlignChargeTarget(body);
            SetPhase_ChargeOrReturnHome(actor, 2, 6);
            return;
        }
        *(struct ContactPosition *)(*(u8 **)(group + 0x1FC) + 0x1C) = center;
        SceneActorTerrain_Subtract(&delta, &center, &actor->base.body.position);
        {
            u8 direction = ((Math_Atan2(delta.x, delta.z) + 0x20) & 0xFF) >> 6 << 6;
            ((u8 *)actor)[0x59] = direction;
            body[0x68E] = direction;
        }
        SetControl_ChargeOrReturnHome(actor, Golem_UpdateRouteMotion, 0x23);
        SetPhase_ChargeOrReturnHome(actor, 2, 1);
    }
    } else
        actor->base.counter2AC++;
}
