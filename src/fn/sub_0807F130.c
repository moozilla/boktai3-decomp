#include "scene_actor_chase_reaction.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_position_search.h"
#include "scene_actor_stimulus.h"

s8 SceneStimulus_FindOverlap(u32, u32, u32, u8, u16);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorPosition_ResolveTaggedPosition(struct SceneActorTaggedPosition *,
                                              const struct SceneActorPositionSource *, void *, s32);
void SceneActorAttachment_RetargetNearestPlayer(void *, const struct ContactPosition *);

#ifdef NONMATCHING
// fuzzy: 88.4%
static inline s8 SceneActorTarget_FindStimulus(struct SceneActorRegistryResetView *owner,
                                               struct SceneActorChaseGroup *group,
                                               struct ContactPosition **source, u32 *tag)
{
    struct ContactPosition *position = &owner->body.position;
    u8 raw =
        SceneStimulus_FindOverlap(position->x, owner->body.position.z, group->common.control.radius,
                                  group->common.control.active, (u16)owner->body.id);
    *source = position;
    *tag = raw;
    return (s8)raw;
}

// confidence=low: starts a route to the tagged position of an overlapping stimulus.
u32 sub_0807F130(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorRegistryControl *control;
    u16 *timer;
    struct ContactPosition *position;
    struct ContactPosition *destination;
    u16 *motionFlags;
    u32 tag;
    u32 started;
    u32 busy = SceneActorStimulus_CheckBusy(actor);
    if (busy)
        return 0;
    group->common.control.mode = busy;
    started = 0;
    if (SceneActorTarget_FindStimulus(actor, group, &position, &tag) < 0)
        goto failed;
    timer = &actor->group.common.control.timer;
    control = &group->common.control;
    if (!*timer) {
        actor->flags2CC &= ~0x141C0;
        control->counter = 0;
        control->field14 = 0;
        /* The control record is reset before the overlap timer is armed. */
        control->timer = 0;
        *timer = 180;
        SceneActorAttachment_ReplaceResponseSlot(object, 0, 1, actor->category);
        SceneActorChase_SetControl(actor, group->common.field43C, 9);
        SceneActorChase_SetBehavior(actor, 11);
        started = 1;
    }
    if (SceneActorStimulus_CaptureMotionFlags(actor, 0x1000, &motionFlags)) {
        struct SceneActorRegistryResetView *owner = object;
        SceneActorContacts_ReleaseNode(owner->group.contactNode);
        *motionFlags &= ~0x1000;
        owner->group.contactNode = NULL;
    }
    SceneActorPosition_ResolveTaggedPosition((void *)control, (void *)&actor->body.id, group->route,
                                             (s8)tag);
    if (*motionFlags & 0x80)
        SceneActorStimulus_ExtendGate(object);
    destination = group->destination;
    SceneActorChase_SnapToRoute(destination, &group->homeRoute);
    /* Fetch the active destination again after reading the terrain. */
    group->destination->y = actor->body.position.y;
    group->destination->pad = 0;

    if (!SceneActorChase_TryRoute(actor, group->route, position, &control->targetPosition)) {
        Callback_InvokeActor(actor, group->fallback);
        return 1;
    }
    {
        u32 mask = 0x10;
        actor->flags2CC |= mask;
    }
    {
        u32 mask = 0x200;
        actor->flags2D4 |= mask;
    }
    actor->field2C2 = 0x20;
    {
        u32 mask = 0x1000000;
        actor->flags2D0 |= mask;
    }
    SceneActorAttachment_RetargetNearestPlayer(object, &control->targetPosition);
    if (started)
        return 1;
failed:
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807F130);
#endif
