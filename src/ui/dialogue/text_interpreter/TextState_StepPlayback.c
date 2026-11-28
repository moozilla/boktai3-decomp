#include "global.h"
#include "ui/dialogue/text_state.h"

const u8 *TextState_RenderNextGlyph(struct TextState *, const u8 *);
void TextState_PopStream(struct TextState *);

int TextState_StepPlayback(struct TextState *state)
{
    const u8 *text;

    state->padE = 0;
    switch (state->style) {
        case 0:
            text = TextState_RenderNextGlyph(state, state->pc);
            state->pc = text;
            if (state->fresh || (!state->style && !*text))
                return 1;
            break;
        case 1:
            text = TextState_RenderNextGlyph(state, state->streams[0]);
            state->streams[0] = text;
            if (state->style == 1 && !*text) {
                TextState_PopStream(state);
                if (!state->style && (state->fresh || !*state->pc))
                    return 1;
            }
            break;
        case 2:
            text = TextState_RenderNextGlyph(state, state->streams[1]);
            state->streams[1] = text;
            if (state->style == 2 && !*text) {
                TextState_PopStream(state);
                if (!state->style && (state->fresh || !*state->pc))
                    return 1;
            }
            break;
        case 3:
            text = TextState_RenderNextGlyph(state, state->streams[2]);
            state->streams[2] = text;
            if (state->style == 3 && !*text) {
                TextState_PopStream(state);
                if (!state->style && (state->fresh || !*state->pc))
                    return 1;
            }
            break;
        case 4:
            text = TextState_RenderNextGlyph(state, state->streams[3]);
            state->streams[3] = text;
            if (state->style == 4 && !*text) {
                TextState_PopStream(state);
                if (!state->style && (state->fresh || !*state->pc))
                    return 1;
            }
            break;
    }
    return 0;
}
