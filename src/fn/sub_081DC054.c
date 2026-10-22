#include "global.h"
const u8 *Text_FindChar(const u8 *, u8);
u16 Text_DecodeGlyphEscape(const u8 *);
void TextWindow_DrawNarrowGlyph(u32, s32, s32, u32);
void TextWindow_DrawWideGlyph(u32, s32, s32, u32);
u32 Text_GetWideGlyphCount(void);
void Menu_DrawText(s32 left, s32 row, const u8 *s)
{
    s32 x = left;
    s32 y = row;
    u32 highlight = 0;
    u32 glyph;
    u32 c;
    for (;;) {
        c = *s;
        c <<= 24;
        if (!c)
            return;
        if ((s32)c >= 0) {
            if (*s == '<') {
                highlight = 1;
                s = Text_FindChar(s, '>');
            } else if (*s == '\n') {
                x = left;
                y += 2;
            } else if (*s == 0x1f) {
                u32 code = (u16)Text_DecodeGlyphEscape(s);
                if (code == 0xFFFF) {
                    glyph = (u16)(*s - 0x20);
                    TextWindow_DrawNarrowGlyph(glyph, x, y, highlight);
                    x++;
                } else if (code & 0xFF00) {
                    u32 mask = 0x7FFF;
                    code &= mask;
                    TextWindow_DrawWideGlyph(code, x, y, highlight);
                    x += 2;
                } else {
                    TextWindow_DrawNarrowGlyph(code, x, y, highlight);
                    x++;
                }
                s++;
            } else {
                glyph = (u16)(*s - 0x20);
                TextWindow_DrawNarrowGlyph(glyph, x, y, highlight);
                x++;
            }
        } else {
            u32 low = *s;
            u32 mask = 0x7f;
            c = mask & low;
            s++;
            low = *s;
            glyph = (c << 8) | low;
            if (glyph >= Text_GetWideGlyphCount())
                x += 2;
            else {
                TextWindow_DrawWideGlyph(glyph, x, y, highlight);
                x += 2;
                if (highlight == 1) {
                    s++;
                    s = Text_FindChar(s, '>');
                    highlight = 0;
                }
            }
        }
        s++;
    }
}
