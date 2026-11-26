#include "scene/actor/scene_actor_behavior.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_chase_shared.h"

void *SceneActorContacts_SelectForActor(void *, u32);
void SceneActorControl_OnIdle(void);
static inline u32 SceneActorReaction_ReadFlags(struct ChaseActor *a, u32 mask, u32 **out)
{
    u32 *f = &a->base.behaviorFlags;
    u32 bits = *f & mask;
    *out = f;
    return bits;
}
static inline void SceneActorReaction_SetBits(u16 *p, u32 mask)
{
    *p |= mask;
}
static inline void SceneActorReaction_SetControl(struct ChaseActor *a, void *p, u32 kind, u32 zero,
                                                 u8 byteZero)
{
    a->base.field2B2 = 1;
    a->base.stepDone = byteZero;
    a->base.controlKind = kind;
    a->base.target = p;
    a->base.field2C8 = zero;
    SceneActorChase_AndWordFlags(&a->base.pendingControlFlags, ~1);
    a->base.pendingControl = zero;
}
// confidence=low: begins a contact reaction after filtering actor and contact flags.
u8 SceneActorReaction_BeginContactResponse(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    void *contact;
    u32 *flags;
    u32 result;
    u8 byteZero;
    s32 reactionMask;
    if (SceneActorBehavior_MotionBits(0x1010, a))
        goto fail;
    if (!SceneActorReaction_ReadFlags(a, 0x1000000, &flags))
        goto fail;
    if (!SceneActorChase_TrackingFlags(&a->base.tracking, 1))
        goto fail;
    {
        u32 mask = 0x80;
        result = *flags & mask;
        if (result)
            goto fail;
    }
    if (((u8 *)a->group.contactNode)) {
        u32 attachmentFlags = (((u8 *)a->group.contactNode))[5];
        if (attachmentFlags & 1)
            goto fail;
        {
            u32 mask = 4;
            mask &= attachmentFlags;
            if (mask)
                goto fail;
        }
    }
    if (!a->base.tracking.limit)
        goto fail;
    result = (u32)SceneActorContacts_SelectForActor(a, 1);
    contact = (void *)result;
    if (contact)
        goto success;
fail:
    return 0;

success: {
    struct SceneActorRegistryControl *control = &group->common.control;
    SceneActorChase_AndWordFlags(flags, 0xFFFEBE3F);
    control->counter = 0;
    byteZero = 0;
    control->field14 = 0;
    control->timer = 0;
}

    group->contactNode = contact;
    SceneActorReaction_SetControl(a, SceneActorControl_OnIdle, 0x10, 0, byteZero);
    SceneActorBehavior_WriteSelector(3, &a->base.state2A8);
    a->base.phase2A9 = 1;
    a->base.behavior2AA = byteZero;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
    SceneActorChase_AndWordFlags(flags, ~0x10);
    reactionMask = 0x800;
    SceneActorReaction_SetBits(&a->base.reactionFlags, reactionMask);
    return 1;
}
