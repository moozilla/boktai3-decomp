#include "contact_collider_manager.h"
#include "ram.h"

struct ContactColliderManager *Collider_GetManager(void)
{
    return (struct ContactColliderManager *)gColliderManager;
}
