#include "scene_actor_contact_response.h"
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
extern const u8 gUnk_0824E114[];

static inline void SceneActorTarget_UpdateContactRoute(struct SceneActorRegistryResetView *actor,
                                                       u16 *motionFlags, void *route,
                                                       struct ContactPosition *source,
                                                       struct ContactPosition *target)
{
    s32 result = Nav_IsPositionInMesh(route, source, target);
    if (!result)
        SceneActorChase_ClearBits(motionFlags, 0x200);
    else {
        result = Terrain_ApplyBlocked(route, source, target);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
        } else
            SceneActorChase_ClearBits(motionFlags, 0x200);
    }
}

// confidence=low: releases a prior contact, checks the route to the target cell,
// and starts the idle reaction when the target is inactive.
u8 SceneActorTarget_UpdateTrackedContact(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorContactTracking *motion;
    struct SceneActorContactTracking *tracking;
    struct SceneActorChaseGroup *group;
    u16 zero;
    u16 *motionFlags;
    u16 *savedFlags;
    void *work;
    void **nodeSlot;
    struct SceneActorContactNode *contact;
    struct SceneActorContactTarget *target;
    u16 cell;
    u32 active;
    u32 *actionFlags;
    work = actor;
    if (SceneActorContact_MotionFlags(SceneActorContact_ActorFromWork(&work), 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->update26C))
        return 1;
    if (!SceneActorContact_TestTrackingOwned((void *)&actor->tracking, 1, &motion))
        return 0;
    motionFlags = SceneActorContact_MotionField(actor);
    {
        u16 flags;
        SCENE_ACTOR_CONTACT_READ_MOTION_FIELD(flags, motionFlags);
        zero = flags & 0x100;
    }
    savedFlags = motionFlags;
    if (zero)
        return 0;
    group = &actor->group;
    tracking = motion;
    nodeSlot = &actor->group.contactNode;
    if (!actor->tracking.limit)
        return 0;
    SCENE_ACTOR_CONTACT_FIND_CONTACT(contact, actor);
    if (!contact)
        return 0;
    if (SceneActorContact_QueryMotionField(actor, 0x1000, &work)) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        SceneActorChase_ClearBits(savedFlags, 0x1000);
        *nodeSlot = (void *)(u32)zero;
    }
    target = contact->object;
    motion->tracking.target = &target->target;
    cell = target->cell->index;
    work = actor;
    SceneActorChase_ClearFlagComponent(SceneActorContact_GateFromWork(&work), 2);
    if (motion->active) {
        if (SceneActorContact_TestTracking(motion, 8)) {
            if (SceneActorContact_SavedTargetCell(actor) != cell) {
                SceneActorTarget_UpdateContactRoute(actor, savedFlags, group->route,
                                                    &actor->body.position,
                                                    &tracking->tracking.target->position);
                *(u16 *)actor->unknown2F8 = cell;
            }
        } else
            SceneActorTarget_ResetContactResponse(actor);
        SceneActorChase_SetReactionFlags(actor, 0x80);
        SceneActorContact_SetTrackingFlags(motion, 0x10);
        return 0;
    }
    active = SceneActorContact_ReadActorFlags(actor, 0x80, &actionFlags);
    if (active)
        return 0;
    *actionFlags |= 0x80;
    SceneActorChase_ClearBits(motionFlags, 0x100);
    SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E114, 0x682);
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_ClearBits(savedFlags, 0x400);
    actor->group.turn.turnsRemaining = -1;
    SceneActorAnimation_SelectDirectionalEntry((void *)SceneActorContact_ActorFromWork(&work), 0);
    actor->group.common.control.timer = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
    SceneActorChase_SetReactionFlags(actor, 0x100);
    return 1;
}
