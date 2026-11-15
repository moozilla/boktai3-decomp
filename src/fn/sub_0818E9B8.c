#include "armor_part_queue.h"

void SpriteColliderGroup_ActivateParts(struct ArmorLaunchGroup *p, s32 requested)
{
    s32 i;

    for (i = 0; i < p->count; i++) {
        struct ArmorLaunchState *parts = (struct ArmorLaunchState *)&p->parts[0].flags;

        if (!parts[i].active) {
            ArmorLaunchGroup_QueuePart(p, &parts[i], 0);
            if (--requested <= 0)
                break;
        }
    }
}
