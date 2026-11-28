#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_PopStream(struct TextState *state)
{
    if (state->depth) {
        state->depth--;
        state->style = state->styles[state->depth];
    }
}
