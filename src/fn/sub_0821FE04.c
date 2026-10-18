#define gColliderManager gUnk_03001688
#include "global.h"
#include "contact_actor.h"
struct ColliderManager {
    u8 unknown0[0x1C];
    struct ContactCollider *prioritySentinel;
    u8 unknown20[0xB2];
    u16 priorityCount;
};
extern struct ColliderManager *gUnk_03001688;
void Collider_InsertPriorityList(struct ContactCollider *node)
{
    struct ContactCollider *entry;
    if (!gUnk_03001688)
        return;
    entry = gUnk_03001688->prioritySentinel->next;
    if (entry) {
        do {
            if (entry == node)
                return;
            entry = entry->next;
        } while (entry);
    }
    entry = gUnk_03001688->prioritySentinel;
    node->next = entry->next;
    entry->next = node;
    gUnk_03001688->priorityCount++;
}
