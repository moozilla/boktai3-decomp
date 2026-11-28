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
                u16 *timerPtr = &actor->hitLockTimer;
                s16 timer = *timerPtr;
                if (timer) {
                    if (((u16)timer >> 2) & 1) {
                        u32 mask = 0x20;
                        mask |= actor->reactionFlags;
                        actor->reactionFlags = mask;
                    }
                    (*timerPtr)--;
                }
                if (control->timer)
                    control->timer--;
            }
        }
        {
            struct SceneActorUpdateContact *contact = (void *)actor;
            if (contact->contact && !SceneActorContacts_ContainsActiveNode(contact->contact)) {
                Callback_InvokeActor(actor, contact->cleanup);
                contact->contact = NULL;
            }
        }
    }
    return 1;
}
