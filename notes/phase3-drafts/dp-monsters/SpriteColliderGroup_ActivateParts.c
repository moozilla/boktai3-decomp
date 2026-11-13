#include "armor_part_queue.h"

void SpriteColliderGroup_ActivateParts(struct ArmorLaunchGroup *p, s32 requested)
{
    s32 i;
    u8 *count, *activeCount;
    u8 active;
    struct ArmorLaunchState *part;
    i = 0;
    if (i >= p->count)
        return;
    active = 1;
    activeCount = &p->activeCount;
    count = &p->count;
    part = (struct ArmorLaunchState *)&p->parts[0].flags;
    for (; i < *count; part++, i++) {
        if (!part->active) {
            ARMOR_QUEUE_PART_ACTION(part, 0, active, active, activeCount);
            if (--requested <= 0)
                break;
        }
    }
}
