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
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The ROM steps a separate s16 pointer through x, y, z and spills i to the stack; this C folds the steps into offsets.
void sub_0823350C(struct HomingSparkSet *p)
{
    s8 i;
    u32 k1 = 8;
    for (i = 0; i < k1; i++) {
        if (p->sparks[i].active) {
            if ((++p->sparks[i].timer) > 5) {
                p->sparks[i].flags |= 1;
                p->sparks[i].active = 0;
            } else {
                s32 weight = k1 - p->sparks[i].timer;
                s32 divisor = weight + 1;
                s16 *position = &p->sparks[i].x;
                do {
                    position[0] = Div((position[0] * weight) + p->targetX, divisor);
                } while (0);
                position++;
                position[0] = Div((position[0] * weight) + p->targetY, divisor);
                position++;
                position[0] = Div((position[0] * weight) + p->targetZ, divisor);
                if (p->sparks[i].timer == 4)
                    SpriteDrawable_SelectTileFrame(&p->sparks[i], p->frames, 9);
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0823350C);
#endif
