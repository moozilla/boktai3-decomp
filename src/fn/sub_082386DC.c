#include "global.h"
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
struct BriefAnimation {
    u8 unknown0[0x744];
    void *frames;
    u32 flags;
    u8 unknown74C[0x24];
    u8 active, timer, firstFrame;
};
#ifdef NONMATCHING
// fuzzy: 84.0%
// Remaining differences: increment lifetime and repeated byte truncation across the timer branch.
#define SpriteEffect_UpdateBriefAnimation sub_082386DC
void SpriteEffect_UpdateBriefAnimation(struct BriefAnimation *p)
{
    if (p->active) {
        u32 timer = ++p->timer;
        if (p->timer > 5) {
            p->flags |= 1;
            p->active = 0;
        } else
            SpriteDrawable_SelectTileFrame(&p->flags, p->frames, (timer >> 2) + p->firstFrame);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_082386DC);
#endif
