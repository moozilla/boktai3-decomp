#include "ui/dialogue/message_window.h"

const u8 *TextState_ProcessTag(struct MessagePlaybackState *, const u8 *);
u16 Text_DecodeGlyphEscape(const u8 *);
int TextState_DrawNarrowGlyph(struct MessagePlaybackState *, u32);
int TextState_DrawWideGlyph(struct MessagePlaybackState *, u32);
int TextState_AdvanceCursor(struct MessagePlaybackState *, int);
void TextState_AdvanceLine(struct MessagePlaybackState *);
u32 Text_GetWideGlyphCount(void);
const u8 *TextState_RenderNextGlyph(struct MessagePlaybackState *p, const u8 *s)
{
    u16 glyph;
    while (1) {
        u32 c = *s;
        c <<= 24;
        if ((s32)c >= 0) {
            c >>= 24;
            if (!c)
                break;
            if (c == '<') {
                s++;
                if (!*s)
                    break;
                s = TextState_ProcessTag(p, s);
                if (p->hold)
                    return s;
                if (!*s)
                    return s;
            } else if (*s == '\n') {
                TextState_AdvanceLine(p);
            } else if (*s == 0x1F) {
                u32 code = (u16)Text_DecodeGlyphEscape(s);
                if (code == 0xFFFF) {
                    glyph = *s - 32;
                    TextState_DrawNarrowGlyph(p, glyph);
                } else if (code & 0xFF00) {
                    code &= 0x7FFF;
                    TextState_DrawWideGlyph(p, code);
                } else
                    TextState_DrawNarrowGlyph(p, code);
                s += 2;
                return s;
            } else {
                glyph = *s - 32;
                TextState_DrawNarrowGlyph(p, glyph);
                s++;
                return s;
            }
        } else {
            u32 first = *s;
            u32 mask = 0x7F;
            u32 second;
            mask &= first;
            s++;
            second = *s;
            glyph = (mask << 8) | second;
            if (glyph >= Text_GetWideGlyphCount())
                TextState_AdvanceCursor(p, 1);
            else
                TextState_DrawWideGlyph(p, glyph);
            s++;
            return s;
        }
        s++;
    }
    return s;
}
