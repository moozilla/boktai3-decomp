#include "scene_actor_update.h"
void SceneActorBehavior_UpdateColliders(struct SceneActorBehaviorView *);
static inline u32 SceneActorBehavior_TestMotionFlags(u32 mask, struct SceneActorBehaviorView *p)
{
    return p->flags2D4 & mask;
}
static inline u32 SceneActorBehavior_TestStatusFlags(u32 mask, struct SceneActorBehaviorView *p)
{
    return p->flags2D0 & mask;
}
static inline u8 Inactive(struct SceneActorBehaviorView *actor)
{
    if (!actor)
        return 0;
    if (SceneActorBehavior_TestMotionFlags(0x40, actor) ||
        SceneActorBehavior_TestStatusFlags(0x20000, actor) || actor->health <= 0)
        return 1;
    return 0;
}
s32 SceneActorBehavior_UpdateTimersAndContacts(struct SceneActorBehaviorView *actor)
{
    struct SceneActorBehaviorView *updateActor = actor;
    u32 inactive;
    actor->counter304 = 0;
    inactive = Inactive(actor);
    if (!inactive) {
        struct SceneActorUpdateControl *control;
        SceneActorBehavior_UpdateColliders(updateActor);
        control = (void *)((u8 *)actor + 0x3D4);
        {
            u32 mask = 0x2000;
            u32 flags;
            flags = actor->flags2D4;
            if (flags & mask)
                control->timer = inactive;
            else {
                struct SceneActorRegistryActor *base = (void *)actor;
                u16 *timerPtr = &base->field2B8;
                s16 timer;
                timer = *timerPtr;
                if (timer) {
                    if (((u16)timer >> 2) & 1) {
                        u32 mask = 0x20;
                        mask |= actor->flags2D6;
                        actor->flags2D6 = mask;
                    }
                    (*timerPtr)--;
                }
                {
                    u8 *timerPtr = &control->timer;
                    if (*timerPtr)
                        (*timerPtr)--;
                }
            }
        }
        {
            struct SceneActorUpdateContact *contact = (void *)actor;
            struct SceneActorContactNode **node;
            node = &contact->contact;
            if (*node) {
                u32 registered = (u8)SceneActorContacts_ContainsActiveNode(*node);
                if (!registered) {
                    void *cleanup = contact->cleanup;
                    Callback_InvokeActor(actor, cleanup);
                    *node = (void *)registered;
                }
            }
        }
    }
    return 1;
}
