#include "display_animation.h"
#define DisplayAnimation_GetFrameBoundary sub_08042524
s32 DisplayAnimation_GetFrameBoundary(struct DisplayAnimation *display, u32 *out)
{
    if (display->kind == 2) {
        struct DisplayKeyframeState *state = (struct DisplayKeyframeState *)((u8 *)display + 0x2C);
        if (state->status & 4) {
            *out = state->frame;
            return 1;
        }
    } else {
        struct DisplaySequenceState *state = (struct DisplaySequenceState *)((u8 *)display + 0x54);
        if (!state->elapsed) {
            *out = state->index;
            return 1;
        }
    }
    return 0;
}
