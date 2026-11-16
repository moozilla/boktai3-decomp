#ifndef GUARD_CONTACT_COLLIDER_MANAGER_H
#define GUARD_CONTACT_COLLIDER_MANAGER_H

#include "contact_collider.h"
#include "task.h"

struct ContactColliderManager {
    struct Task task;
    struct ContactCollider *source, *pending;
    struct ContactCollider *sourceTail, *pendingTail;
    struct ContactCollider sourceHead, pendingHead;
    u16 sourceCount, pendingCount;
};

struct ContactColliderManager *Collider_GetManager(void);

#endif
