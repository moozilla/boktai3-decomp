#include "frame_display.h"
#include "word_flags.h"

void ActorDebris_Hide(struct FrameDisplay *display)
{
    if (display) {
        WordFlags_Set(&display->flags, 1);
        WordFlags_Set(&display->sprite.flags, 1);
    }
}
