#include "scene/actor/scene_actor_update.h"
#include "scene/motion/scene_actor_availability.h"
#include "engine/word_flags.h"
void SceneActorBehavior_UpdateColliders(struct RoomActor *);
s32 SceneActorBehavior_UpdateTimersAndContacts(struct RoomActor *actor)
{
    struct RoomActor *updateActor = actor;
    u32 inactive;
    actor->counter304 = 0;
    inactive = SceneActor_ActionsBlocked((void *)actor);
    if (!inactive) {
        struct SceneActorUpdateControl *control;
        SceneActorBehavior_UpdateColliders(updateActor);
        control = (void *)((u8 *)actor + 0x3D4);
        {
            u32 mask = 0x2000;
            u32 flags;
            flags = actor->motionFlags;
            if (flags & mask)
                control->timer = inactive;
            else {
                struct ChaseActor *base = (void *)actor;
                u16 *timerPtr = &base->base.hitLockTimer;
                s16 timer;
                timer = *timerPtr;
                if (timer) {
                    if (((u16)timer >> 2) & 1) {
                        u32 mask = 0x20;
                        mask |= actor->reactionFlags;
                        actor->reactionFlags = mask;
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
