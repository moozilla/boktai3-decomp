#include "global.h"
#include "ui/dialogue/text_state.h"

const u8 *TextState_GetActiveStream(struct TextState *state)
{
    switch (state->style) {
        case 0:
            return state->pc;
        case 1:
            return state->streams[0];
        case 2:
            return state->streams[1];
        case 3:
            return state->streams[2];
        case 4:
            return state->streams[3];
        default:
            return 0;
    }
}
