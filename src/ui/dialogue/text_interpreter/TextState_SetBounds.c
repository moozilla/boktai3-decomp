#include "ui/dialogue/message_window.h"

void TextState_SetBounds(void *playback, u32 left, u32 top, u32 width, u32 height)
{
    struct MessagePlaybackState *state = playback;

    state->left = left;
    state->top = top;
    state->width = width;
    state->height = height;
    state->x = left;
    state->y = top;
}
