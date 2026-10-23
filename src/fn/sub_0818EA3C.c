#include "global.h"

// MATCHING_CONCESSION: register-pinning
struct SpriteColliderPartState {
    u8 flags, unknown1[4], active, counter;
    u8 unknown7[0x1DD];
};
struct SpriteColliderGroup {
    u8 unknown0[0x5c3];
    u8 count, activeCount;
};
void Sound_PlaySongChecked(u32);
void SpriteColliderGroup_ResetActiveParts(struct SpriteColliderGroup *p)
{
    s32 i;
    s32 changed = 0;
    struct SpriteColliderPartState *part;
    u8 *count;
    u32 five, one;
    register u8 total asm("r5");
    i = 0;
    total = p->count;
    if (i >= total)
        goto done;
    one = 1;
    five = 5;
    count = &p->count;
    part = (struct SpriteColliderPartState *)((u8 *)p + 0x119);
    do {
        if (part->active) {
            part->active = one;
            part->counter = five;
            part->flags = one | part->flags;
            changed = 1;
        }
        part++;
        i++;
    } while (i < *count);
done:
    if (changed)
        Sound_PlaySongChecked(0x13e);
}
