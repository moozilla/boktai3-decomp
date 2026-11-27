#include "player/effects/frame_display.h"
#include "engine/word_flags.h"

s32 ActorDebris_Update(struct FrameDisplay *display)
{
    if (WordFlags_Test(&display->flags, 1)) {
        display->sprite.flags |= 1;
        return 0;
    }
    FrameDisplay_UpdateSprite(display);
    display->frameCounter++;
}
