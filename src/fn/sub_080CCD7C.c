// confidence=low: existing Golem state interpretation retained.
#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "player.h"
u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
void SceneActor_AccelerateForTerrain(struct SceneActorRegistryResetView *);
u8 Actor_StepTowardTargetAxis(struct SceneActorRegistryResetView *, void *, u32, u32, s32, u32);
s32 Math_Atan2(s32, s32);
s32 SceneActorTerrain_CompareNeighborHeight(s32, u32, u32);
void Skeleton_StepPosition(struct ContactPosition *, u32);
void sub_080C47DC(void *);
void SceneActorReaction_SlideBack(void);
void Golem_UpdateChargeStop(void);
void Golem_UpdateChargeRecovery(void);
struct GolemChargeBounds {
    u8 unknown0[0x1C];
    struct ContactPosition home;
};
struct GolemBody {
    u8 unknown0[0x625];
    u8 flag625;
    u8 unknown626[2];
    u32 flags628;
    u8 unknown62C[0x680 - 0x62C];
    s16 stamina680, charge682;
    u8 unknown684[2];
    u16 counter686;
    u16 charge688;
    u8 unknown68A[4];
    u8 direction68E, unknown68F, direction690, direction691;
    u8 unknown692[2];
    struct ContactPosition home694, target69C;
};

static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}

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
    struct SceneActorRegistryResetView *actor = object;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

/* Turned too far off the charge heading: face the new direction and spend 10 stamina; out of
 * stamina, stop the charge (behavior 3), else aim at the route's home and recover (behavior 2). */
static inline void Golem_TurnAway(struct GolemBody *body)
{
    body->direction691 = body->direction690;
    ((struct SceneActorRegistryResetView *)body)->body.direction = body->direction690;
    body->stamina680 -= 10;
    if (body->stamina680 <= 0) {
        SceneActorChase_SetControl(body, Golem_UpdateChargeStop, 0x27);
        Golem_SetBehavior(body, 3);
        return;
    }
    body->target69C =
        ((struct GolemChargeBounds *)((struct SceneActorRegistryResetView *)body)->group.route)
            ->home;
    {
        void *target = Golem_UpdateChargeRecovery;
        SceneActorChase_SetControl(body, target, 0x26);
    }
    Golem_SetBehavior(body, 2);
}

static inline void Golem_Recover(struct GolemBody *body)
{
    body->stamina680 -= 30;
    body->direction691 = body->direction690;
    SceneActorChase_SetState((struct SceneActorRegistryResetView *)body, 0, 2, 7);

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

static inline void Golem_Retarget(struct SceneActorRegistryResetView *actor, struct GolemBody *body)
{
    s32 d = Wrap(actor->tracking.heading - body->direction68E);
    if ((d < 0 ? -d : d) <= 15 && actor->terrainCell != actor->previousTerrainCell) {
        body->target69C = body->home694;
        sub_080C47DC(body);
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
void Golem_UpdateChargePursuit(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct GolemBody *body = actor->context;
    struct ContactPosition diff;
    void *route;
    if (TakeFlag2B1(actor))
        body->flag625 = 1;
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (body->flag625 == 1 && body->charge682 == 0) {
        SceneActorPlacement_PlayVariantEventSound((void *)actor, 4);
        body->flag625 = 0;
    }
    SceneActor_AccelerateForTerrain(actor);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *partner = actor->tracking.target;
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
            if (Actor_StepTowardTargetAxis(actor, route, 24, 24, body->charge682, 0)) {
                s32 distance;
                s32 d;
                body->target69C = body->home694;
                SceneActorTerrain_Subtract(&diff, &body->target69C, &actor->body.position);
                distance = diff.x * diff.x + diff.z * diff.z;
                {
                    u8 *facing = &body->direction68E;
                    body->flag625 = 1;
                    d = Wrap(actor->tracking.heading - *facing);
                }
                if (d < 0)
                    d = -d;
                if (d <= 15 && distance <= 0xFFFE && body->charge682 > 199) {
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
                        sub_080C47DC(body);
                    } else if ((u32)(a - 0x34) <= 0x18)
                        sub_080C47DC(body);
                }
                if (Golem_TurnGap(body->direction690, &body->direction68E) > 31) {
                    Golem_TurnAway(body);
                    return;
                }
                SceneActorTerrain_Subtract(&diff, &actor->tracking.target->position,
                                           &actor->body.position);
                distance = diff.x * diff.x + diff.z * diff.z;
                if (distance <= 0xFFFE && body->charge682 > 199) {
                    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
                        struct SceneActorChaseTarget *partner = actor->tracking.target;
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
            if (Golem_TurnGap(body->direction690, &body->direction68E) > 31) {
                Golem_TurnAway(body);
                return;
            }
        } else {
            if (Actor_StepTowardTargetAxis(actor, route, 24, 24, body->charge682, 0)) {
                body->target69C = body->home694;
                sub_080C47DC(body);
            } else
                Golem_Retarget(actor, body);
            if (Golem_TurnGap(body->direction690, &body->direction68E) > 31) {
                Golem_TurnAway(body);
                return;
            }
        }
    }
    if (body->stamina680 > 0)
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
        actor->field2C2 = 0x20;
        goto advance;
    } else {
        {
            u32 mask = 0x10;
            actor->flags2CC |= mask;
        }
        actor->field2C2 = 0x40;
        SetCommand((void *)&group->saved, actor->entryTarget228, 7, 60);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->pad = 1;
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                      group->destination)) {
            Callback_InvokeActor(actor, group->fallback);
            return;
        }
    }
advance:
    body->counter686++;
    actor->counter2AC++;
}
