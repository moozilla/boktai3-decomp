#include "global.h"
#include "contact_actor.h"
struct ColliderManager {
    u8 unknown0[0x1C];
    struct ContactCollider *prioritySentinel;
    u8 unknown20[0xB2];
    u16 priorityCount;
};
extern struct ColliderManager *gColliderManager;
void Collider_InsertPriorityList(struct ContactCollider *node)
{
    struct ContactCollider *entry;
    if (!gColliderManager)
        return;
    entry = gColliderManager->prioritySentinel->next;
    if (entry) {
        do {
            if (entry == node)
                return;
            entry = entry->next;
        } while (entry);
    }
    entry = gColliderManager->prioritySentinel;
    node->next = entry->next;
    entry->next = node;
    gColliderManager->priorityCount++;
}
