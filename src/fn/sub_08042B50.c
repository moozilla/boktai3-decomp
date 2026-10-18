#include "contact_actor.h"
s32 ContactActor_HideSlot(struct ContactActorPool *, struct ContactActor *, s32);
void SceneActors_StartDisappearEffect(void *);
void ContactActorPool_HideSlots(struct ContactActorPool *pool)
{
    struct ContactActor *slot = pool->slots;
    s32 i = 0;
    do {
        ContactActor_HideSlot(pool, slot, i);
        i++;
        slot++;
    } while (i <= 15);
    SceneActors_StartDisappearEffect(&pool->retreatCenter);
    pool->state = 0;
}
