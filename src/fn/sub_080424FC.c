#include "display_animation.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *display)
{
    if (display->kind == 1)
        return display->finished;
    {
        struct DisplayKeyframeState *state = (struct DisplayKeyframeState *)((u8 *)display + 0x2C);
        if (state->status & 2)
            return 1;
        return 0;
    }
}
