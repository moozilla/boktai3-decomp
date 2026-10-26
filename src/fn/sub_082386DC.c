#include "global.h"
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
struct BriefAnimation {
    u8 unknown0[0x744];
    void *frames;
    u32 flags;
    u8 unknown74C[0x24];
    u8 active, timer, firstFrame;
};
#define SpriteEffect_UpdateBriefAnimation sub_082386DC
void SpriteEffect_UpdateBriefAnimation(struct BriefAnimation *p)
{
    if (p->active) {
        if (++p->timer > 5) {
            p->flags |= 1;
            p->active = 0;
        } else
            SpriteDrawable_SelectTileFrame(&p->flags, p->frames, p->firstFrame + (p->timer >> 2));
    }
}
