#include "contact_actor.h"
s32 ContactActor_HideSlot(struct ContactActorPool *unused, struct ContactActor *slot,
                          s32 unusedIndex)
{
    u32 *flags = (u32 *)&slot->sprite;
    struct ContactCollider *collider;
    *flags |= 1;
    collider = (struct ContactCollider *)((u8 *)flags - 0x64);
    collider->flags |= 4;
    ActorShadow_Disable(slot->shadow);
    /* The observed callers ignore the unspecified success result. */
}
