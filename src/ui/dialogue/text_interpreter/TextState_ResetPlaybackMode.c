#include "system/game_state/game_state.h"
#include "ui/dialogue/text_state.h"

void TextState_ResetPlaybackMode(struct TextState *state)
{
    state->entered = 0;
    {
        s32 mode = (s16)gGameState->unk12;

        state->mode = mode;
    }
}
