#define TimedSprite_UpdateFrames sub_0823674C
#include "global.h"
#include "display_animation.h"
struct TimedSprite {
    u8 unknown0[0x48];
    struct DisplaySequenceSprite sprite;
    u8 unknown74[0x1c];
    u8 active;
    u8 unknown91[5];
    s16 ticks;
};
void sub_0823674C(struct TimedSprite *effect)
{
    s16 ticks = effect->ticks;
    u32 frame;
    struct DisplaySequenceSprite *sprite;
    if ((u16)ticks <= 7)
        frame = (u16)ticks >> 2;
    else
        frame = ((u16)ticks >> 2) + 4;
    sprite = &effect->sprite;
    sprite->frame = frame;
    if ((u16)++effect->ticks > 23) {
        effect->sprite.flags |= 1;
        effect->active = 0;
    }
}
