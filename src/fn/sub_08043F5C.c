#include "contact_actor.h"
#define ContactActorPool_RequestClear sub_08043F5C
void ContactActorPool_RequestClear(struct ContactActorPool *pool)
{
    pool->lifetime = 600;
}
