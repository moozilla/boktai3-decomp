#include "global.h"
#include "engine/video/display_animation.h"
struct TimedSprite {
    u8 unknown0[0x48];
    struct DisplaySequenceSprite sprite;
    u8 unknown74[0x1c];
    u8 active;
    u8 unknown91[5];
    u16 ticks;
};
void TimedSprite_UpdateFrames(struct TimedSprite *effect)
{
    u32 frame;
    struct DisplaySequenceSprite *sprite;
    if (effect->ticks <= 7)
        frame = effect->ticks >> 2;
    else
        frame = (effect->ticks >> 2) + 4;
    sprite = &effect->sprite;
    sprite->frame = frame;
    if (++effect->ticks > 23) {
        effect->sprite.flags |= 1;
        effect->active = 0;
    }
}
