#include "player/player_burst/frame_display.h"
#include "engine/shared/word_flags.h"

void ActorDebris_Hide(struct FrameDisplay *display)
{
    if (display) {
        WordFlags_Set(1, &display->flags);
        WordFlags_Set(1, &display->sprite.flags);
    }
}
