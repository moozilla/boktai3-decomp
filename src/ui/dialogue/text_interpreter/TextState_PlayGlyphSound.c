#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_state.h"
extern const s16 gUnk_086055E0[];
void Sound_PlaySong(int);
void TextState_PlayGlyphSound(struct TextState *state, int glyph)
{
    if (state->skip)
        return;
    if (state->busy)
        return;
    if ((state->mode <= 1 || state->instant) && state->toggle)
        goto clear;
    if (glyph && glyph != gSilentGlyph)
        Sound_PlaySong(gUnk_086055E0[state->voice]);
    if (state->toggle) {
    clear:
        state->toggle = 0;
    } else {
        state->toggle++;
    }
}
