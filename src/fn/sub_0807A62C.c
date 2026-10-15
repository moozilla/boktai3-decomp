// MATCHING_CONCESSION: register-pinning
#include "scene_actor_update.h"
void SceneActorBehavior_UpdateColliders(struct SceneActorBehaviorView *);
static inline u8 Inactive(struct SceneActorBehaviorView *actor)
{
    u32 offset;
    if (!actor)
        return 0;
    {
        u32 mask = 0x40;
        offset = 0x2D4;
        asm("" : "+r"(offset));
        if (*(u16 *)((u8 *)actor + offset) & mask)
            return 1;
    }
    {
        u32 mask = 0x20000;
        offset -= 4;
        {
            u32 flags = *(u32 *)((u8 *)actor + offset);
            if (flags & mask)
                return 1;
        }
    }
    if (actor->health <= 0)
        return 1;
    return 0;
}
s32 SceneActorBehavior_UpdateTimersAndContacts(struct SceneActorBehaviorView *actor)
{
    register struct SceneActorBehaviorView *updateActor asm("r2") = actor;
    u32 inactive;
    /* The caller supplies a valid actor; the inlined availability test retains its null case. */
    actor->counter304 = 0;
    inactive = Inactive(actor);
    if (!inactive) {
        struct SceneActorUpdateControl *control;
        SceneActorBehavior_UpdateColliders(updateActor);
        control = (void *)((u8 *)actor + 0x3D4);
        {
            u32 mask = 0x2000;
            u32 flags;
            {
                /* Retain the initialized control offset across the flag load for allocation. */
                register u32 offset asm("r2") = 0x3D4;
                flags = actor->flags2D4;
                asm("" : "+r"(flags) : "r"(offset));
            }
            if (flags & mask)
                control->timer = inactive;
            else {
                struct SceneActorRegistryActor *base = (void *)actor;
                register u32 timerOffset asm("r2") = 0x2B8;
                u16 *timerPtr;
                s16 timer;
                asm("" : "+r"(timerOffset));
                timerPtr = (void *)((u8 *)base + timerOffset);
                timer = *timerPtr;
                if (timer) {
                    if (((u16)timer >> 2) & 1) {
                        u32 mask = 0x20;
                        asm("" : "+r"(mask));
                        actor->flags2D6 |= mask;
                    }
                    (*timerPtr)--;
                }
                {
                    register u32 timerOffset asm("r2") = 0x1F6;
                    u8 *timerPtr;
                    asm("" : "+r"(timerOffset));
                    timerPtr = (u8 *)control + timerOffset;
                    if (*timerPtr)
                        (*timerPtr)--;
                }
            }
        }
        {
            register u32 contactOffset asm("r3") = 0x45C;
            struct SceneActorUpdateContact *contact = (void *)actor;
            struct SceneActorContactNode **node;
            node = (void *)((u8 *)actor + contactOffset);
            if (*node) {
                u32 registered = (u8)SceneActorContacts_ContainsActiveNode(*node);
                if (!registered) {
                    register u32 cleanupOffset asm("r1") = 0x618;
                    void *cleanup;
                    {
                        register void **cleanupPointer asm("r0");
                        cleanupPointer = (void *)((u8 *)contact + cleanupOffset);
                        cleanup = *cleanupPointer;
                    }
                    sub_0824923C(actor, cleanup);
                    *node = (void *)registered;
                }
            }
        }
    }
    return 1;
}
