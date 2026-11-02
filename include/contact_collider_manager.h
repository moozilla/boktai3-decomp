#ifndef GUARD_CONTACT_COLLIDER_MANAGER_H
#define GUARD_CONTACT_COLLIDER_MANAGER_H

#include "contact_collider.h"

struct ContactColliderManager {
    u8 task[0x18];
    struct ContactCollider *source, *pending;
    struct ContactCollider *sourceTail, *pendingTail;
    struct ContactCollider sourceHead, pendingHead;
    u16 sourceCount, pendingCount;
};

struct ContactColliderManager *Collider_GetManager(void);

#endif
