#include "contact_actor.h"
#define ContactActorPool_HideSlots sub_08042B50
#define ContactActor_HideSlot      sub_08042B28
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
    sub_080224E4(pool->scriptState);
    pool->state = 0;
}
