#include "global.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u32 frame;
    const u8 *pc, *streams[4];
    u8 pad30[0xD8];
    u8 depth, styles[7];
};
const u8 *TextState_ProcessTag(struct TextState *, const u8 *);
u16 Text_DecodeGlyphEscape(const u8 *);
int TextState_DrawNarrowGlyph(struct TextState *, u32);
int TextState_DrawWideGlyph(struct TextState *, u32);
int TextState_AdvanceCursor(struct TextState *, int);
void TextState_AdvanceLine(struct TextState *);
u32 Text_GetWideGlyphCount(void);
const u8 *TextState_RenderNextGlyph(struct TextState *p, const u8 *s)
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
                if (p->padE)
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
                    u32 mask = 0x7FFF;
                    code &= mask;
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
