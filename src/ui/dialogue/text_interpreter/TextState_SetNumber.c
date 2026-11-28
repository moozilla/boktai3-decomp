#include "ui/dialogue/message_window.h"

u32 TextState_SetNumber(void *playback, int index, int number)
{
    struct MessagePlaybackState *state = playback;

    state->numbers[index] = number;
}
