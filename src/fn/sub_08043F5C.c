#include "contact_actor.h"
void ContactActorPool_RequestClear(struct ContactActorPool *pool)
{
    pool->lifetime = 600;
}
