#include "armor_part_queue.h"

// MATCHING_CONCESSION: register-pinning
void SpriteColliderGroup_ActivateParts(struct ArmorLaunchGroup *p, s32 requested)
{
    s32 i;
    u8 *count, *activeCount;
    u8 active;
    register u32 stride asm("ip");
    struct ArmorLaunchState *part;
    asm("" ::: "r8");
    i = 0;
    if (i >= p->count)
        return;
    active = 1;
    activeCount = &p->activeCount;
    count = &p->count;
    part = (struct ArmorLaunchState *)&p->parts[0].flags;
    stride = sizeof(*part);
    for (; i < *count; part = (void *)((u8 *)part + stride), i++) {
        if (!part->active) {
            ARMOR_QUEUE_PART_ACTION(part, 0, active, active, activeCount);

            if (--requested <= 0)
                break;
        }
    }
}
