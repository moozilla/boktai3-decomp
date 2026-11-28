#include "player/effects/frame_display.h"

void ActorDebris_Show(struct FrameDisplay *display)
{
    if (display != 0) {
        u32 mask = ~1;
        display->flags &= mask;
        display->sprite.flags &= mask;
    }
}
