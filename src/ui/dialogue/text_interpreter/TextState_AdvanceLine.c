#include "global.h"
#include "ui/dialogue/text_state.h"
void TextState_AdvanceLine(struct TextState *state)
{
    if (state->wrap != 1) {
        if (state->y - state->top < state->height) {
            state->x = state->left;
            state->y += 2;
        }
    } else {
        state->wrap = 0;
    }
}
