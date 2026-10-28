#include "global.h"
#include "contact_actor.h"
#include "ram.h"
struct ColliderManager {
    u8 unknown0[0x1C];
    struct ContactCollider *prioritySentinel;
    u8 unknown20[0xB2];
    u16 priorityCount;
};
void Collider_InsertPriorityList(struct ContactCollider *node)
{
    struct ContactCollider *entry;
    if (!((struct ColliderManager *)gColliderManager))
        return;
    entry = ((struct ColliderManager *)gColliderManager)->prioritySentinel->next;
    if (entry) {
        do {
            if (entry == node)
                return;
            entry = entry->next;
        } while (entry);
    }
    entry = ((struct ColliderManager *)gColliderManager)->prioritySentinel;
    node->next = entry->next;
    entry->next = node;
    ((struct ColliderManager *)gColliderManager)->priorityCount++;
}
