#include "contact_collider_manager.h"
#include "ram.h"

void Collider_ResetManagerLists(void)
{
    struct ContactColliderManager *manager = (struct ContactColliderManager *)gColliderManager;
    if (manager != NULL) {
        manager->source = &manager->sourceHead;
        manager->pending = &manager->pendingHead;
        manager->sourceHead.next = NULL;
        manager->pendingHead.next = NULL;
        manager->sourceTail = &manager->sourceHead;
        manager->pendingTail = &manager->pendingHead;
        manager->sourceCount = 0;
        manager->pendingCount = 0;
    }
}
