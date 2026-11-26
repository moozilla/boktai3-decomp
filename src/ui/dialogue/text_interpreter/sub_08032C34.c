#include "ui/dialogue/message_window.h"

void TextState_Reset(void *playback)
{
    struct MessagePlaybackState *state = playback;
    state->palette = 0;
    state->depth = 0;
    state->stream = 0;
    if (state->wrap != 2)
        state->wrap = 0;
}
