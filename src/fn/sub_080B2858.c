#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reaction.h"
extern const u8 gUnk_0824E230[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
u8 Vorn_SelectReachableEscapeCell(struct SceneActorRegistryResetView *, u32);
u32 SceneActor_DispatchContextAction(void *, u32);
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
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(object);
    struct SceneActorChaseTracking *tracking = &actor->tracking;
    u32 cell;
    struct ContactPosition target;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    cell = SceneActorChase_CellAt(&tracking->target->position);
    if (cell == actor->terrainCell ||
        SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[0])->flags,
                               0x20000)) {
        SceneActor_DispatchContextAction(actor->context, 0);
        return 0;
    }
    SceneActorChase_CellCenter(cell, &target);
    if (!SceneActorChase_TryRoute(object, group->route, &actor->body.position, &target)) {
        Callback_InvokeActor(actor, group->fallback);
        return 0;
    }
    return 1;
}
u32 Vorn_SelectRetreatRoute(struct SceneActorRegistryResetView *actor, u32 reset)
{
    struct VornRetreatContext *body = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    u8 old;
    if ((!reset) && body->timer)
        return 0;
    body->timer = 60;
    SceneActorChase_ClearActorFlags(actor, 0x10);
    Vorn_AdjustContextFlags(body, 0x10, 0x40);
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E230, 0x486);
    body->requests++;
    if (actor->tracking.distance < body->nearDistance) {
        old = actor->body.direction;
        actor->body.direction = actor->tracking.heading + 0x80;
        if (!Vorn_SelectReachableEscapeCell(actor, old))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    } else if (actor->tracking.distance > body->farDistance) {
        old = actor->body.direction;
        {
            u32 random = Vorn_RandomSample();
            actor->body.direction = Vorn_ScatterDirection(&actor->tracking.heading, random);
        }
        if (!actor->tracking.target)
            actor->tracking.target =
                &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        if ((!Vorn_TryTrackedCell(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    } else {
        old = actor->body.direction;
        {
            u32 random = Vorn_RandomSample();
            actor->body.direction = Vorn_ScatterDirection(&actor->tracking.heading, random);
        }
        if (!actor->tracking.target)
            actor->tracking.target =
                &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        if ((!Vorn_TryTrackedCell(actor)) && (!Vorn_SelectReachableEscapeCell(actor, old)))
            return 0;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetBehavior(actor, 0x15);
    }
    {
        u32 bit = 4;
        u32 *field = &actor->flags2D0;
        u32 value = *field;
        value |= bit;
        *field = value;
    }
    return 1;
}
