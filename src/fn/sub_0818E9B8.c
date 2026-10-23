#include "global.h"

#define SpriteColliderGroup_ActivateParts sub_0818E9B8
// MATCHING_CONCESSION: register-pinning
struct SpriteColliderPartState {
    u8 flags, unknown1[4], active, counter;
    u8 unknown7[0x1DD];
};
struct SpriteColliderGroup {
    u8 unknown0[0x5c3];
    u8 count, activeCount;
};
void SpriteColliderGroup_ActivateParts(struct SpriteColliderGroup *p, s32 requested)
{
    s32 i;
    u8 *count, *activeCount;
    u32 one;
    register u32 stride asm("ip");
    struct SpriteColliderPartState *part;
    asm("" ::: "r8");
    i = 0;
    if (i >= p->count)
        return;
    one = 1;
    activeCount = &p->activeCount;
    count = &p->count;
    part = (struct SpriteColliderPartState *)((u8 *)p + 0x119);
    stride = sizeof(*part);
    for (; i < *count; part = (void *)((u8 *)part + stride), i++) {
        if (!part->active) {
            part->active = one;
            part->counter = 0;
            part->flags |= one;
            (*activeCount)++;
            if (--requested <= 0)
                break;
        }
    }
}
