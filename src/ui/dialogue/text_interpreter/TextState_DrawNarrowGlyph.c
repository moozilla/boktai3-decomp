#include "global.h"
#include "ui/dialogue/text_state.h"
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
