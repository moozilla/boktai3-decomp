#include "global.h"

#ifdef NONMATCHING
// fuzzy: 81.7%
// foundry draft (worker best 81.7%); what differs is mostly register allocation and instruction order
#define Collider_ScheduleUpdates sub_0811B094
struct ScheduledCollider {
    u32 flags;
    u8 unknown4[0x30];
    s16 updateDelay;
};
struct ColliderLink {
    struct ScheduledCollider *collider;
    struct ColliderLink *previous, *next;
};
struct ColliderScheduler {
    u32 unknown0;
    struct ColliderLink *head;
    u32 unknown8;
    s32 count;
};
extern u32 gUnk_030053F4, gUnk_030053F0;
s32 Div(s32, s32);
void sub_0811B094(void *unused, struct ColliderScheduler *scheduler)
{
    struct ColliderLink *link;
    s32 enabled, quota, count, divisor, delay;
    u32 one;
    u32 mask = 0x800;
    if ((gUnk_030053F4 | gUnk_030053F0) & mask)
        return;
    count = scheduler->count;
    if (count <= 0)
        return;
    link = scheduler->head;
    enabled = 0;
    if (count <= 4)
        quota = 1;
    else {
        if (count <= 9) {
            quota = 3;
            count += 2;
            divisor = 3;
        } else {
            quota = 4;
            count += 3;
            divisor = 4;
        }
        count = Div(count, divisor);
    }
    delay = count - 1;
    if (link) {
        one = 1;
        do {
            struct ScheduledCollider *collider = link->collider;
            if (collider->flags & one) {
                collider->flags |= 2;
                collider->updateDelay = 0;
            } else if (enabled >= quota) {
                collider->flags |= 2;
            } else if (collider->updateDelay > 0) {
                collider->flags |= 2;
                collider->updateDelay--;
            } else {
                collider->updateDelay = delay;
                collider->flags &= ~2;
                enabled++;
            }
            link = link->next;
        } while (link);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811B094);
#endif
