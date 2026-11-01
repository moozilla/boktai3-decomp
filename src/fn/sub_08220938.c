#include "contact_collider.h"

void PositionList_UpdateTranslatedCoordinates(struct ContactCollider **, struct ContactCollider **);
s32 Box_Overlaps(struct ContactCollider *, struct ContactCollider *);

void Collision_ProcessPairs(struct ContactCollider **listA, struct ContactCollider **listB)
{
    struct ContactCollider *a;
    struct ContactCollider *b;
    struct ContactCollider *cur;
    struct ContactCollider *other;
    s32 (*overlaps)(struct ContactCollider *, struct ContactCollider *) = Box_Overlaps;
    u32 suppressCallback, priorityMask;
    PositionList_UpdateTranslatedCoordinates(listA, listB);
    cur = *listA;
    while (cur != 0) {
        a = cur;
        cur = a->next;
        b = *listB;
        if (b != 0) {
            suppressCallback = 2;
            do {
                other = b;
                b = other->next;
                if ((a->contactFlags & other->contactFlags) != 0)
                    continue;
                if ((other->flags & 4) != 0)
                    continue;
                if (other->collisionId != 0 && other->collisionId == a->collisionId)
                    continue;
                if ((a->flags & 1) == 0 && (other->flags & 1) != 0)
                    continue;
                if ((a->flags & 0x1000) != 0 && (other->flags & 0x1000) == 0)
                    continue;
                if (overlaps(a, other) == 0)
                    continue;
                other->cost = 0;
                priorityMask = 0x200000;
                if ((a->statRewardFlags & priorityMask) != 0) {
                    if ((a->flags & suppressCallback) == 0 && other->callback != 0)
                        other->callback(a, other, other->owner);
                    if ((other->flags & suppressCallback) == 0 && a->callback != 0)
                        a->callback(a, other, a->owner);
                } else {
                    if ((other->flags & suppressCallback) == 0 && a->callback != 0)
                        a->callback(a, other, a->owner);
                    if ((a->flags & suppressCallback) == 0 && other->callback != 0)
                        other->callback(a, other, other->owner);
                }
            } while (b != 0);
        }
    }
}
