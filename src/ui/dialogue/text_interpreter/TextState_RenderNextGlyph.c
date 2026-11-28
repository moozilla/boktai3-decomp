#include "ui/dialogue/message_window.h"

const u8 *TextState_ProcessTag(struct MessagePlaybackState *, const u8 *);
u16 Text_DecodeGlyphEscape(const u8 *);
int TextState_DrawNarrowGlyph(struct MessagePlaybackState *, u32);
int TextState_DrawWideGlyph(struct MessagePlaybackState *, u32);
int TextState_AdvanceCursor(struct MessagePlaybackState *, int);
void TextState_AdvanceLine(struct MessagePlaybackState *);
u32 Text_GetWideGlyphCount(void);
const u8 *TextState_RenderNextGlyph(struct MessagePlaybackState *state, const u8 *text)
{
    u16 glyph;
    u32 c;

    for (;;) {
        c = *text;
        c <<= 24;
        if ((s32)c >= 0) {
            c >>= 24;
            if (!c)
                break;
            if (c == '<') {
                text++;
                if (!*text)
                    break;
                text = TextState_ProcessTag(state, text);
                if (state->hold || !*text)
                    return text;
            } else if (*text == '\n') {
                TextState_AdvanceLine(state);
            } else if (*text == 0x1F) {
                u32 code = (u16)Text_DecodeGlyphEscape(text);

                if (code == 0xFFFF) {
                    glyph = *text - ' ';
                    TextState_DrawNarrowGlyph(state, glyph);
                } else if (code & 0xFF00) {
                    code &= 0x7FFF;
                    TextState_DrawWideGlyph(state, code);
                } else {
                    TextState_DrawNarrowGlyph(state, code);
                }
                text += 2;
                return text;
            } else {
                glyph = *text - ' ';
                TextState_DrawNarrowGlyph(state, glyph);
                text++;
                return text;
            }
        } else {
            u32 high = *text & 0x7F;
            u32 low;

            text++;
            low = *text;
            glyph = (high << 8) | low;
            if (glyph >= Text_GetWideGlyphCount())
                TextState_AdvanceCursor(state, 1);
            else
                TextState_DrawWideGlyph(state, glyph);
            text++;
            return text;
        }
        text++;
    }
    return text;
}
