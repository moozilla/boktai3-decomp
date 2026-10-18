#define gSpriteWidths  gUnk_03004D90
#define gSpriteHeights gUnk_03004D20
#include "global.h"
#include "display_animation.h"
extern const u8 gUnk_03004D90[], gUnk_03004D20[];
void Sprite_ApplyAnimation(struct DisplaySequenceSprite *s, const u8 *resource)
{
    if (resource) {
        s->frame = 0;
        s->frameIndex = 0;
        s->elapsed = 0;
        s->width = gUnk_03004D90[resource[1]];
        s->height = gUnk_03004D20[resource[1]];
        s->offsetX = 0;
        s->offsetY = 0;
        s->attributes =
            (s->attributes & 0x1c00) | (((resource[1] & 3) << 14) | ((resource[1] & 12) << 28));
        if (resource[0] & 0x10)
            s->attributes |= 0x2000;
        s->resource = resource;
    }
}
