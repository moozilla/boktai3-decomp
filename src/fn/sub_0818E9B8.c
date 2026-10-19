
#ifdef NONMATCHING
// fuzzy: 80.0%
// foundry draft (worker best 80.0%); what differs is mostly register allocation and instruction order
#define SpriteColliderGroup_ActivateParts sub_0818E9B8
#include "global.h"
struct SpriteColliderPartState {
    u8 flags, unknown1[4], active, counter;
    u8 unknown7[0x1DD];
};
struct SpriteColliderGroup {
    u8 unknown0[0x5c3];
    u8 count, activeCount;
};
void sub_0818E9B8(struct SpriteColliderGroup *p, s32 requested)
{
    s32 i;
    struct SpriteColliderPartState *part;
    i = 0;
    if (i >= p->count)
        return;
    part = (struct SpriteColliderPartState *)((u8 *)p + 0x119);
    for (; i < p->count; part++, i++) {
        if (!part->active) {
            u32 one = 1;
            part->active = one;
            part->counter = 0;
            part->flags |= one;
            p->activeCount++;
            if (--requested <= 0)
                break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818E9B8);
#endif
