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
// fuzzy: 55.9%
// confidence=low: starts a route to the tagged position of an overlapping stimulus.
u32 sub_0807F130(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorRegistryControl *control = &group->common.control;
    struct ContactPosition *position;
    struct ContactPosition *destination;
    s8 tag;
    u32 started;
    u32 busy = SceneActorStimulus_CheckBusy(actor);
    if (busy)
        return 0;
    control->mode = busy;
    started = 0;
    position = &actor->body.position;
    tag = SceneStimulus_FindOverlap(position->x, position->z, control->radius,
                                    control->active, (u16)actor->body.id);
    if (tag < 0)
        return 0;
    if (!control->timer) {
        actor->flags2CC &= ~0x141C0;
        control->counter = 0;
        control->field14 = 0;
        control->timer = 180;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
        SceneActorChase_SetControl(actor, group->common.field43C, 9);
        SceneActorChase_SetBehavior(actor, 11);
        started = 1;
    }
    if (actor->flags2D4 & 0x1000) {
        SceneActorContacts_ReleaseNode(group->contactNode);
        actor->flags2D4 &= ~0x1000;
        group->contactNode = (void *)busy;
    }
    SceneActorPosition_ResolveTaggedPosition((void *)control, (void *)&actor->body.id, group->route, tag);
    if (actor->flags2D4 & 0x80) {
        group->common.gate.counter += 300;
        if (group->common.gate.counter > group->common.gate.duration)
            group->common.gate.counter = group->common.gate.duration;
    }
    destination = group->destination;
    SceneActorChase_SnapToRoute(destination, &group->homeRoute);
    destination->y = position->y;
    destination->pad = 0;
    if (!SceneActorChase_TryRoute(actor, group->route, position, &control->targetPosition)) {
        Callback_InvokeActor(actor, group->fallback);
        return 1;
    }
    actor->flags2CC |= 0x10;
    actor->flags2D4 |= 0x200;
    actor->field2C2 = 0x20;
    actor->flags2D0 |= 0x01000000;
    SceneActorAttachment_RetargetNearestPlayer(actor, &control->targetPosition);
    if (started)
        return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807F130);
#endif
