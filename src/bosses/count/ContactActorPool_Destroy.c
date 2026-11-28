#include "scene/collision/contact_actor.h"

void ContactActor_DestroySlot(struct ContactActorPool *, struct ContactActor *, s32);

u32 ContactActorPool_Destroy(struct ContactActorPool *pool)
{
    struct ContactActor *slot = pool->slots;
    s32 i;
    for (i = 0; i < 16; i++, slot++)
        ContactActor_DestroySlot(pool, slot, i);
    return 0;
}
