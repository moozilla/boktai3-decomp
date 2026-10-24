#include "global.h"
struct FlashSprite {
    u8 pad0[0x744];
    void *frames;
    u8 drawable[0x770 - 0x748];
    u8 active;
    u8 timer;
    u8 baseFrame;
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
// confidence=low: steps a six-frame animation four ticks per frame, then hides the sprite.
void sub_0813B950(struct FlashSprite *p)
{
    if (p->active) {
        if (++p->timer > 5) {
            *(u32 *)p->drawable |= 1;
            p->active = 0;
        } else
            SpriteDrawable_SelectTileFrame(p->drawable, p->frames, p->baseFrame + (p->timer >> 2));
    }
}
