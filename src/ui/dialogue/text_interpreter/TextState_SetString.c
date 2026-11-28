#include "ui/dialogue/message_window.h"

u32 TextState_SetString(void *playback, u32 index, u32 string)
{
    struct MessagePlaybackState *state = playback;

    state->strings[index] = (const u8 *)string;
}
