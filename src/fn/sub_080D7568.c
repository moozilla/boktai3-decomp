#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_contacts.h"

struct MummyContactContext {
    u8 unknown0[0x6F2];
    u16 flags;
};
struct MummyContactTarget {
    struct SceneActorChaseTarget target;
    u8 unknown14[8];
    struct SceneActorChasePlayerCell *cell;
};
s32 Mummy_CanPassMovementThreshold(void *);
u8 Mummy_TryDirectPursuit(void *);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline u8 Mummy_TestContext(struct MummyContactContext *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}
static inline u8 Mummy_TestContextOwned(struct MummyContactContext *context, u32 mask,
                                        struct SceneActorChaseFlags16 **field)
{
    struct SceneActorChaseFlags16 *flags = (void *)&context->flags;
    u32 value = flags->value;
    value &= mask;
    *field = flags;
    if (value)
        return 1;
    return 0;
}

static inline u32 Mummy_MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline void Mummy_SetTrackingFlags(struct SceneActorChaseTracking *tracking, u8 bits)
{
    tracking->flags |= bits;
}
static inline u32 Mummy_StatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 Mummy_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

static inline void Mummy_SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    SceneActorChase_ResetStateEntry(actor);
}

static inline u8 Mummy_Arrive(void *obj, struct SceneActorChaseTracking *tracking)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
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
    if (cell != actor->terrainCell &&
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
        SceneActorChase_SetControl(actor, actor->entryTarget228, 1);
        Mummy_SetBehavior(actor, 5);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        (group->destination)->y = SceneActorChase_HeightAt(
            SceneActorChase_CellAt(group->destination), group->destination);
        (group->destination)->pad = 0;
    }
    {
        u8 saved = actor->body.direction;
        SceneActorChase_BeginTurn(obj, saved, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->field2C2 = 0x20;
    return 0;
reroute: {
    SceneActorChase_CellCenter(cell, &target);
    if (SceneActorChase_TryRoute(obj, group->route, &actor->body.position, &target))
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

u8 sub_080D7568(struct SceneActorRegistryResetView *actor)
{
    struct MummyContactContext *context = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget **target;
    struct SceneActorContactNode *contact;
    u16 cell;
    struct SceneActorChaseFlags16 *contextFlags;
    if (Mummy_MotionFlags(actor, 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->update26C))
        return 1;
    if (Mummy_MotionFlags(actor, 0x100))
        return 0;
    if (!Mummy_MotionFlags(actor, 0x200))
        return 0;
    if (!Mummy_StatusFlags(actor, 0x1000000))
        return 0;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    target = Mummy_BeginContactTracking(&actor->tracking, 2);
    if (!actor->tracking.limit)
        goto restore;
    contact = SceneActorContacts_SelectForActor((void *)actor, 2);
    if (!contact)
        goto restore;
    (*target) = contact->object;
    cell = ((struct MummyContactTarget *)(*target))->cell->index;
    if (*(u32 *)actor->unknown100) {
        if (SceneActorChase_TestField(&actor->flags2C0, 2))
            (*target) = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
        else if (!((*target)))
            goto threshold;
        {
            s32 x = ((*target))->position.x - actor->body.position.x;
            s32 z = ((*target))->position.z - actor->body.position.z;
            actor->tracking.heading = Math_Atan2(x, z);
            actor->tracking.distance = (x * x) + (z * z);
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
            if ((*(u16 *)actor->unknown2F8) != cell) {
                SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                         &((*target))->position);
                *(u16 *)actor->unknown2F8 = cell;
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
    if (actor->field2C2 != 64)
        return 0;
    if (group->common.control.counter != gTrackedPlayerId[0])
        return 0;
    SceneActorChase_SetReactionFlags(actor, 0x100);
restore:
    if (Mummy_TestContextOwned(context, 0x4000, &contextFlags)) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
        SceneActorChase_ClearFlagComponent(contextFlags, 0x4000);
    }

    return 0;
}
