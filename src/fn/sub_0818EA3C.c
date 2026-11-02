// MATCHING_CONCESSION: register-pinning
#include "armor_part_queue.h"

void Sound_PlaySongChecked(u32);
void SpriteColliderGroup_ResetActiveParts(struct ArmorLaunchGroup *p)
{
    s32 i;
    s32 changed = 0;
    struct ArmorLaunchState *part;
    u8 *count;
    u32 resetAction, active;
    register u8 total asm("r5");
    i = 0;
    total = p->count;
    if (i < total) {
        active = 1;
        resetAction = 5;
        count = &p->count;
        part = (struct ArmorLaunchState *)&p->parts[0].flags;
        do {
            if (part->active) {
                part->active = active;
                part->action = resetAction;
                part->flags = active | part->flags;
                changed = 1;
            }
            part++;
            i++;
        } while (i < *count);
    }
    if (changed)
        Sound_PlaySongChecked(0x13e);
}
