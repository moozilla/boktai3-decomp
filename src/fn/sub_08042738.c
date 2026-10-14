#include "display_animation.h"
s32 DisplayAnimation_SetPriority(struct DisplayAnimation *display, u32 priority)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1) {
        struct DisplaySequenceSprite *sprite =
            (struct DisplaySequenceSprite *)((u8 *)display + 0x28);
        sprite->priority = priority;
    } else {
        struct DisplayKeyframeState *state = (struct DisplayKeyframeState *)((u8 *)display + 0x2C);
        state->priority = priority;
    }
    return 0;
}
