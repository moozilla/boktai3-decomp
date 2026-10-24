#include "global.h"
struct HomingSpark {
    u32 flags;
    u8 pad4[0x18 - 4];
    s16 x, y, z;
    u8 pad1E[0x28 - 0x1E];
    u8 active, timer;
    u8 pad2A[2];
};
struct HomingSparkSet {
    u8 pad0[0x8C];
    s16 targetX, targetY, targetZ;
    u8 pad92[0xA0 - 0x92];
    void *frames;
    struct HomingSpark sparks[8];
};
s32 Div(s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
// confidence=low: pulls each live spark toward the target by 1/(9 - t) of the gap per frame,
// switches it to frame 9 at tick 4 and hides it after tick 5.
static inline s16 *AdvanceCoordinate(s16 *position, struct HomingSparkSet *p, s32 axis, s32 weight,
                                     s32 divisor)
{
    s32 product = *position * weight;
    s16 *target = &p->targetX;
    *position = Div(product + target[axis], divisor);
    return position + 1;
}

void sub_0823350C(struct HomingSparkSet *p)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        /* The first spark in this view is the indexed spark in the set. */
        struct HomingSparkSet *slot = (void *)((u8 *)p + i * sizeof(struct HomingSpark));
        if (slot->sparks[0].active) {
            if ((++slot->sparks[0].timer) > 5) {
                p->sparks[i].flags |= 1;
                slot->sparks[0].active = 0;
            } else {
                s32 weight = 8 - slot->sparks[0].timer;
                s32 divisor = weight + 1;
                s16 *position = &slot->sparks[0].x;
                position = AdvanceCoordinate(position, p, 0, weight, divisor);
                position = AdvanceCoordinate(position, p, 1, weight, divisor);
                AdvanceCoordinate(position, p, 2, weight, divisor);
                if (slot->sparks[0].timer == 4)
                    SpriteDrawable_SelectTileFrame(&p->sparks[i], p->frames, 9);
            }
        }
    }
}
