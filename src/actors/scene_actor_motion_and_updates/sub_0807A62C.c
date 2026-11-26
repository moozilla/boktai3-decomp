#include "actors/scene_actor_motion_and_updates/scene_actor_update.h"
#include "engine/shared/scene_actor_availability.h"
#include "engine/shared/word_flags.h"
void SceneActorBehavior_UpdateColliders(struct RoomActor *);
static inline u8 Inactive(struct RoomActor *actor)
{
    if (!actor)
        return 0;
    if (SceneActor_MaskedHalfword(0x40, &actor->motionFlags) ||
        Flags_MaskedWord(0x20000, &actor->statusFlags) || actor->health <= 0)
        return 1;
    return 0;
}
s32 SceneActorBehavior_UpdateTimersAndContacts(struct RoomActor *actor)
{
    struct RoomActor *updateActor = actor;
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
