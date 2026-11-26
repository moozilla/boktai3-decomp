#include "global.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u8 pad18[4];
    const u8 *pc;
    u8 pad20[0xE8];
    u8 depth, styles[7];
};
void TextState_PlayGlyphSound(struct TextState *, int);
void TextState_AdvanceLine(struct TextState *);
int TextState_AdvanceCursor(struct TextState *, int);
void TextWindow_DrawNarrowGlyph(int, int, int, int);
int TextState_DrawNarrowGlyph(struct TextState *p, u16 c)
{
    TextState_PlayGlyphSound(p, c);
    if (p->wrap != 2 && p->x >= p->left + p->width) {
        TextState_AdvanceLine(p);
        p->wrap = 1;
    }
    TextWindow_DrawNarrowGlyph(c, p->x, p->y, p->palette);
    TextState_AdvanceCursor(p, 0);
    p->lastWidth = 0;
}
