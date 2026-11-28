#include "ui/dialogue/message_window.h"


void TextState_RunSelectedProcedures(void *playback)
{
    struct MessagePlaybackState *state = playback;
    u32 i;

    for (i = 0; i < state->selectedCount; i++)
        Script_Call(state->choices[state->selectedChoices[i]], 0);
}
