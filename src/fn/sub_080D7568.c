#include "global.h"
#include "mummy.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_contacts.h"

struct MummyContactTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};
s32 Mummy_CanPassMovementThreshold(void *);
u8 Mummy_TryDirectPursuit(void *);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline u8 Mummy_TestContext(struct Mummy *context, u32 mask)
{
    if (context->flags6F2 & mask)
        return 1;
    return 0;
}
static inline u8 Mummy_TestContextOwned(struct Mummy *context, u32 mask,
                                        struct SceneActorChaseFlags16 **field)
{
    struct SceneActorChaseFlags16 *flags = (void *)&context->flags6F2;
    u32 value = flags->value;
    value &= mask;
    *field = flags;
    if (value)
        return 1;
    return 0;
}

static inline u32 Mummy_MotionFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2D4 & mask;
}

static inline void Mummy_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags |= bits;
}
static inline u32 Mummy_StatusFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2D0 & mask;
}
static inline u32 Mummy_ActorFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2CC & mask;
}

static inline void Mummy_SetBehavior(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    SceneActorChase_ResetStateEntry(actor);
}

static inline u8 Mummy_Arrive(void *obj, struct SceneActorChaseTracking *tracking)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    struct SceneActorChaseGroup *group = SceneActorChase_GroupOf(obj);
    s32 cell, player;
    const u16 *randomSamples = gRandomTable;
    void **callback;
    SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *t = tracking->target;
        if (!t)
            goto invalid_target;
        player = SceneActorChase_PlayerSlot(t);
        if (player < 0) {
        invalid_target:
            Callback_InvokeActor(actor, group->fallback);
            return 0;
        }
        cell = ((struct SceneActorChasePlayer *)gUnknown_02000580[player])->cell->index;
    } else {
        struct SceneActorChaseTarget *t = tracking->target;
        cell = SceneActorChase_CellAt(&t->position);
        player = 0;
    }
    if (cell != actor->base.terrainCell &&
        !SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[player])->flags,
                                0x20000))
        goto reroute;
    if (SceneActorChase_MotionFlags(actor, 0x400))
        return 0;
    {
        u32 bits = 4;
        group->common.gate.flags &= ~bits;
    }
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetControl(actor, group->common.gate.control, 0x1A);
        Mummy_SetBehavior(actor, 12);
    } else {
        SceneActorChase_SetControl(actor, actor->base.entryTarget228, 1);
        Mummy_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
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
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->base.body.position, &target))
        goto success;
}
    callback = &group->fallback;
    Callback_InvokeActor(actor, *callback);
    return 0;
success:
    return 1;
}

static inline struct SceneActorChaseTarget **
Mummy_BeginContactTracking(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags |= bits;
    return &tracking->target;
}

u8 sub_080D7568(struct ChaseActor *actor)
{
    struct Mummy *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget **target;
    struct SceneActorContactNode *contact;
    u16 cell;
    struct SceneActorChaseFlags16 *contextFlags;
    if (Mummy_MotionFlags(actor, 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.update26C))
        return 1;
    if (Mummy_MotionFlags(actor, 0x100))
        return 0;
    if (!Mummy_MotionFlags(actor, 0x200))
        return 0;
    if (!Mummy_StatusFlags(actor, 0x1000000))
        return 0;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    target = Mummy_BeginContactTracking(&actor->base.tracking, 2);
    if (!actor->base.tracking.limit)
        goto restore;
    contact = SceneActorContacts_SelectForActor((void *)actor, 2);
    if (!contact)
        goto restore;
    (*target) = contact->object;
    cell = ((struct MummyContactTarget *)(*target))->cell->index;
    if (*(u32 *)actor->base.unknown100) {
        if (SceneActorChase_TestField(&actor->base.flags2C0, 2))
            (*target) = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        else if (!((*target)))
            goto threshold;
        {
            s32 x = ((*target))->position.x - actor->base.body.position.x;
            s32 z = ((*target))->position.z - actor->base.body.position.z;
            actor->base.tracking.heading = Math_Atan2(x, z);
            actor->base.tracking.distance = (x * x) + (z * z);
        }
    threshold:
        if (!((u8)Mummy_CanPassMovementThreshold(actor))) {
            if (Mummy_TestContext(context, 0x1400))
                return 0;
            Mummy_Arrive(actor, (void *)target);
            return 0;
        }

        if (!Mummy_TryDirectPursuit(actor))
            return 0;
        if (SceneActorChase_TrackingFlags((void *)target, 8)) {
            if ((actor->base.savedTargetCell) != cell) {
                SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                         &((*target))->position);
                actor->base.savedTargetCell = cell;
            }
        } else
            SceneActorTarget_ResetContactResponse(actor);
        SceneActorChase_SetReactionFlags(actor, 0x80);
        Mummy_SetTrackingFlags((void *)target, 0x10);
        return 1;
    }
    if (Mummy_ActorFlags(actor, 0x80) && (!Mummy_MotionFlags(actor, 0x400)))
        goto restore;
    if (Mummy_TestContext(context, 0x1400))
        goto restore;
    if (actor->base.field2C2 != 64)
        return 0;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    SceneActorChase_SetReactionFlags(actor, 0x100);
restore:
    if (Mummy_TestContextOwned(context, 0x4000, &contextFlags)) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->base.category);
        SceneActorChase_ClearFlagComponent(contextFlags, 0x4000);
    }

    return 0;
}
