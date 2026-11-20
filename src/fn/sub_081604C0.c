#include "frame_display.h"
#include "word_flags.h"

s32 ActorDebris_Update(struct FrameDisplay *display)
{
    if (WordFlags_Test(&display->flags, 1)) {
        WordFlags_Set(1, &display->sprite.flags);
        return 0;
    }
    FrameDisplay_UpdateSprite(display);
    display->frameCounter++;
}
