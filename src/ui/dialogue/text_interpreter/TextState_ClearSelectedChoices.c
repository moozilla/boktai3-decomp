#include "ui/dialogue/message_window.h"

void TextState_ClearSelectedChoices(void *playback)
{
    struct MessagePlaybackState *state = playback;

    state->selectedCount = 0;
}
