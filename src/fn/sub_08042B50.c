#include "contact_actor.h"
s32 ContactActor_HideSlot(struct ContactActorPool *, struct ContactActor *, s32);
void sub_080224E4(void *);
void ContactActorPool_HideSlots(struct ContactActorPool *pool)
{
    struct ContactActor *slot = pool->slots;
    s32 i = 0;
    do {
        ContactActor_HideSlot(pool, slot, i);
        i++;
        slot++;
    } while (i <= 15);
    sub_080224E4(&pool->retreatCenter);
    pool->state = 0;
}
