#include "scene_actor_contact_response.h"

void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
extern const u8 gUnk_0824E230[];

u8 Vorn_UpdateContactResponse(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorContactTracking *motion;
    u16 zero;
    u16 *motionFlags;
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
    if (zero)
        return 0;
    nodeSlot = &actor->group.contactNode;
    if (!actor->tracking.limit)
        return 0;
    SCENE_ACTOR_CONTACT_FIND_CONTACT(contact, actor);
    if (!contact)
        return 0;
    if (SceneActorContact_QueryMotionField(actor, 0x1000, &work)) {
        SceneActorContacts_ReleaseNode(*nodeSlot);
        SceneActorChase_ClearBits(work, 0x1000);
        *nodeSlot = (void *)zero;
    }
    target = contact->object;
    motion->tracking.target = &target->target;
    cell = target->cell->index;
    work = actor;
    SceneActorChase_ClearFlagComponent(SceneActorContact_GateFromWork(&work), 2);
    if (motion->active) {
        if (SceneActorContact_TestTracking(motion, 8)) {
            if (SceneActorContact_SavedTargetCell(actor) != cell)
                SceneActorContact_SaveTargetCell(actor, cell);
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
    SceneActorReaction_ResetResponseState(&actor->group.saved, gUnk_0824E230, 0x689);
    SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    SceneActorChase_SetState(actor, 3, 1, 0);
    SceneActorChase_ClearBits(motionFlags, 0x400);
    actor->group.turn.turnsRemaining = -1;
    SceneActorAnimation_SelectDirectionalEntry(SceneActorContact_ActorFromWork(&work), 0);
    actor->group.common.control.timer = 0xB4;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
    SceneActorChase_SetReactionFlags(actor, 0x100);
    return 1;
}
