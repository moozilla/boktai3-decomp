#include "monsters/armor/armor_part_queue.h"

u32 Armor_IsSequenceIncomplete(struct ArmorLaunchGroup *group)
{
    u32 r = 0;
    if (group->activeCount < group->count) {
        r = 1;
    }
    return r;
}
