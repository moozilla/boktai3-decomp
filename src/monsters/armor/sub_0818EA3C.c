#include "monsters/armor/armor_part_queue.h"

void Sound_PlaySongChecked(u32);
void SpriteColliderGroup_ResetActiveParts(struct ArmorLaunchGroup *p)
{
    s32 i;
    s32 changed = 0;

    for (i = 0; i < p->count; i++) {
        struct ArmorLaunchState *parts = (struct ArmorLaunchState *)&p->parts[0].flags;

        if (parts[i].active) {
            ArmorLaunchState_SetAction(&parts[i], 1, 5);
            changed = 1;
        }
    }
    if (changed)
        Sound_PlaySongChecked(0x13e);
}
