#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_PushStream(struct TextState *state, int style)
{
    if (state->depth <= 6) {
        state->styles[state->depth] = state->style;
        state->depth++;
        state->style = style;
    }
}
